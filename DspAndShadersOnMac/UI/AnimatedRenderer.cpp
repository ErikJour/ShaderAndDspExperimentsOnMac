//
//  AnimatedRenderer.cpp
//  DspAndShadersOnMacApp
//
//  Created by Erik Jourgensen on 11/19/26.
//
#include "AnimatedRenderer.hpp"

AnimatedRenderer::AnimatedRenderer(MTL::Device* device, MyRenderer* audioRenderer) : mDevice{device->retain()},                         mAudioRenderer{audioRenderer}{
    
    mCommandQueue           = mDevice->newCommandQueue();
    mFrameBoundarySemaphore = dispatch_semaphore_create(kMaxFramesInFlight);
    mCurrentFrameIndex      = 0;
    BuildShaders();
    BuildBuffers();
}

AnimatedRenderer::~AnimatedRenderer() {
    mCommandQueue->release();
    mDevice->release();
}

void AnimatedRenderer::BuildShaders()
{
    using NS::StringEncoding::UTF8StringEncoding;
   
    NS::Error*  error   = nullptr;
    NS::String* libPath = NS::Bundle::mainBundle()->resourcePath()
                              ->stringByAppendingString(MTLSTR("/default.metallib"));
    MTL::Library* lib   = mDevice->newLibrary(NS::URL::fileURLWithPath(libPath), &error);
    if (!lib) {
        std::cout << "Failed to load metallib at path " << libPath->utf8String();
        if (error) std::cout << ": " << error->localizedDescription()->utf8String();
        std::cout << "\n";
        assert(false);
    }

 
    MTL::Function* vertexFunction   = lib->newFunction( NS::String::string("vertexMain", UTF8StringEncoding));
    
    MTL::Function* fragmentFunction = lib->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding));
   
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();

    renderPipelineDescriptor->setVertexFunction( vertexFunction );
 
    renderPipelineDescriptor->setFragmentFunction( fragmentFunction );
  
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );

 
    mPipelineState = mDevice->newRenderPipelineState( renderPipelineDescriptor, &error );
    if ( !mPipelineState ){
        std::cout << error->localizedDescription()->utf8String()<< "\n";
        assert( false );
       }
  
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
    lib->release();
}

void AnimatedRenderer::BuildBuffers()
{
 
    NS::UInteger page             = getpagesize();
    NS::UInteger raw              = (NS::UInteger)GLOBAL_WIDTH * GLOBAL_HEIGHT * sizeof(game_vertex);
    NS::UInteger vertexBufferSize = (raw + page - 1) & ~(page - 1);

 
    for (int i = 0; i < kMaxFramesInFlight; i++) {
        
        //We allocate a pointer to hold a buffer
        auto *buf     = (VertexBuffer *)malloc(sizeof(VertexBuffer));
        
        //The buffer's position and color can be stored in here
        buf->vertices = (game_vertex *)mmap(nullptr, vertexBufferSize,
                                            PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE | MAP_ANON, -1, 0);
        
        //Put this into a game render command
        mGameRenderCommands.vertexBuffer[i] = buf;

        //We create a metal buffer that takes in position and color, is the size in bytes of pages (I think) multipled by aspect ratio
        MTL::Buffer* metalBuffer = mDevice->newBuffer(buf->vertices,
                                                      vertexBufferSize,
                                                      MTL::ResourceStorageModeShared,
                                                      ^(void* p, NS::UInteger len) { munmap(p, len); });
        
        //Add the metal buffer to the vertex buffer array
        mVertexBuffers[i] = NS::TransferPtr(metalBuffer);
    }

}

void AnimatedRenderer::Draw(MTK::View *view) {
    
    //Read about this in the Metal book
    dispatch_semaphore_wait(mFrameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    //Current frame in the triple buffering I am guessing
    uint32_t frameIndex   = mCurrentFrameIndex;

    uint32_t vertexCount  = 0;

    game_vertex *vertices = mGameRenderCommands.vertexBuffer[frameIndex]->vertices;

    uint32_t squareAStart = vertexCount;
    
    float sideLength = 150;
    
    drawSquare(vertices,
               &vertexCount,
               sideLength,
               blue,
               0,
               0);
    
    uint32_t squareACount = vertexCount - squareAStart;
   

    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //get the current command buffer object to encode commands for execution in the GPU
    auto* commandBuffer = mCommandQueue->commandBuffer();
    
    //get the current render pass descriptor that will be populated with different render targets and their information
    auto* renderPassDescriptor = view->currentRenderPassDescriptor();
    
    MTL::RenderPassColorAttachmentDescriptor* color0 =
        renderPassDescriptor->colorAttachments()->object(0);

    color0->setLoadAction(MTL::LoadActionClear);
    color0->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
    
    auto* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    
   
    CGSize drawableSize = view->drawableSize();
    
    MTL::Viewport viewPort = { 0.0, 0.0,
                            drawableSize.width,
                            drawableSize.height,
                            0.0,
                            1.0 };
    
    renderCommandEncoder->setViewport(viewPort);
    
    renderCommandEncoder->setRenderPipelineState(mPipelineState);
    renderCommandEncoder->setVertexBuffer(mVertexBuffers[mCurrentFrameIndex].get(), 0, 0);
    
    // PASS UNIFORMS TO FRAGMENT SHADER AT INDEX 11
    mAudioData           = mAudioRenderer->data.outputData;
    mUniforms.audioData += mAudioData * 0.05;;
        
    mUniforms.viewportSize = (vector_float2){   GLOBAL_WIDTH,
                                                GLOBAL_HEIGHT };
    
    mUniforms.materialId = MAT_BOTTOM_LEFT;
    
    renderCommandEncoder->setVertexBytes(&mUniforms,
                                         sizeof(Uniforms),
                                         11);
    
    renderCommandEncoder->setFragmentBytes(&mUniforms,
                                           sizeof(Uniforms),
                                           11);
    
    renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         squareAStart,
                                         squareACount);

    
    renderCommandEncoder->endEncoding();
    
    commandBuffer->presentDrawable(view->currentDrawable());
        
    uint32_t nextIndex = mCurrentFrameIndex + 1;
    if (nextIndex > 2) {
        nextIndex = 0;
    }

    mCurrentFrameIndex = nextIndex;

    dispatch_semaphore_t semaphore = mFrameBoundarySemaphore;
    
    commandBuffer->addCompletedHandler([semaphore](MTL::CommandBuffer*) {
        dispatch_semaphore_signal(semaphore);
    });
    
    commandBuffer->commit();

    pool->release();

}

void AnimatedRenderer::MouseDown(float x, float y)
{
    setBottomLeftClicked(true);
}

void AnimatedRenderer::MouseUp(float x, float y)
{
    setBottomLeftClicked(false);
    mAudioRenderer->onOff = false;
}

void AnimatedRenderer::setBottomLeftClicked(bool clicked)
{
    //Check bottom left square for click
    mUniforms.clickedSquare.bottomLeft  = clicked;
    //Turn on the audio
    mAudioRenderer->onOff               = true;
    mAudioRenderer->osc.setFrequency(349.23);
}



