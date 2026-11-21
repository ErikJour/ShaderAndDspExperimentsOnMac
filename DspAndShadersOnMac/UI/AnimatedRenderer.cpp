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
    //============================================================================
    // Shader Library & Render Pipeline Setup
    //============================================================================
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

    //Setting up shader function names and logging error
    MTL::Function* vertexFunction = lib->newFunction( NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function* fragmentFunction = lib->newFunction( NS::String::string("fragmentMain", UTF8StringEncoding));
  
    //object to specify the rendering configuration state
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    //in a render pass we use this vertex function
    renderPipelineDescriptor->setVertexFunction( vertexFunction );
    //in a render pass we use this fragment function
    renderPipelineDescriptor->setFragmentFunction( fragmentFunction );
    // format of the pixel buffer
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );

    //create a render pipeline state
    mPipelineState = mDevice->newRenderPipelineState( renderPipelineDescriptor, &error );
    if ( !mPipelineState ){
        std::cout << error->localizedDescription()->utf8String()<< "\n";
        assert( false );
       }

    //release objects no longer needed
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
    lib->release();
}

void AnimatedRenderer::BuildBuffers()
{
    //============================================================================
    // Buffer Setup
    //============================================================================
    NS::UInteger page             = getpagesize();
    NS::UInteger raw              = (NS::UInteger)GLOBAL_WIDTH * GLOBAL_HEIGHT * sizeof(game_vertex);
    NS::UInteger vertexBufferSize = (raw + page - 1) & ~(page - 1);

    //================================Fix this
    //Triple buffering
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
    
    //The various shapes I draw=========================================================
    //Bottom left
    uint32_t squareAStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, blue, -128, -128);
    uint32_t squareACount = vertexCount - squareAStart;
    //Bottom right
    uint32_t squareBStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, red,   128,  -128);
    uint32_t squareBCount = vertexCount - squareBStart;
    //Top left
    uint32_t squareCStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, green,   -128,  128);
    uint32_t squareCCount = vertexCount - squareCStart;
    //Top right
    uint32_t squareDStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, magenta,   128,  128);
    uint32_t squareDCount = vertexCount - squareDStart;
    //=====================================================================================
    
    //====================================================================================

    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

    //get the current command buffer object to encode commands for execution in the GPU
    auto* commandBuffer = mCommandQueue->commandBuffer();
    
    //get the current render pass descriptor that will be populated with different render targets and their information
    auto* renderPassDescriptor = view->currentRenderPassDescriptor();
    MTL::RenderPassColorAttachmentDescriptor* color0 =
        renderPassDescriptor->colorAttachments()->object(0);

    color0->setLoadAction(MTL::LoadActionClear);
    color0->setClearColor(MTL::ClearColor::Make(0.1, 0.1, 0.1, 1.0));
    
    auto* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    
    //==========
    
    // Viewport using actual drawable size
    CGSize drawableSize = view->drawableSize();
    MTL::Viewport viewPort = { 0.0, 0.0, drawableSize.width, drawableSize.height, 0.0, 1.0 };
    renderCommandEncoder->setViewport(viewPort);
    
    renderCommandEncoder->setRenderPipelineState(mPipelineState);
    renderCommandEncoder->setVertexBuffer(mVertexBuffers[mCurrentFrameIndex].get(), 0, 0);
    
    // PASS UNIFORMS TO FRAGMENT SHADER AT INDEX 11
    mUniforms.audioData     = mAudioRenderer->data.outputData;
    mUniforms.viewportSize = (vector_float2){ 512.0f, 512.0f };
    
    //Bottom Left Square
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
    //Bottom right square
    mUniforms.materialId = MAT_BOTTOM_RIGHT;
    renderCommandEncoder->setVertexBytes(&mUniforms,
                                         sizeof(Uniforms),
                                         11);
    renderCommandEncoder->setFragmentBytes(&mUniforms,
                                           sizeof(Uniforms),
                                           11);
    renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         squareBStart,
                                         squareBCount);
    //Top left
    mUniforms.materialId = MAT_TOP_LEFT;
    renderCommandEncoder->setVertexBytes(&mUniforms,
                                         sizeof(Uniforms),
                                         11);
    renderCommandEncoder->setFragmentBytes(&mUniforms,
                                           sizeof(Uniforms),
                                           11);
    renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         squareCStart,
                                         squareCCount);
    //Top right
    mUniforms.materialId = MAT_TOP_RIGHT;
    renderCommandEncoder->setVertexBytes(&mUniforms,
                                         sizeof(Uniforms),
                                         11);
    renderCommandEncoder->setFragmentBytes(&mUniforms,
                                           sizeof(Uniforms),
                                           11);
    renderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         squareDStart,
                                         squareDCount);

    
    //YOU SHALL NOT ENCODE ANYMORE - end encoding
    renderCommandEncoder->endEncoding();
    
    //tell gpu we got something to draw
    commandBuffer->presentDrawable(view->currentDrawable());
        
    //Frame and looping back
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
    std::cout << "X value is: " << x << std::endl;
    setBottomLeftClicked(true);
}

void AnimatedRenderer::MouseUp(float x, float y)
{
    std::cout << "X value is: " << x << std::endl;
    setBottomLeftClicked(false);
}

void AnimatedRenderer::setBottomLeftClicked(bool clicked)
{
    //Check bottom left square for click
    mUniforms.clickedSquare.bottomLeft  = clicked;
    //Turn on the audio
//    mAudioRenderer->onOff               = true;
//    mAudioRenderer->osc.setFrequency(349.23);
}



