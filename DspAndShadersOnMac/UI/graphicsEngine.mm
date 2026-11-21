//
//  graphicsEngine.mm
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/6/26.
//
#import "graphicsEngine.h"
#include "../Geometries/SquareGeo.h"
#include "../Geometries/TriangleGeo.h"

vector_float4 blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
vector_float4 red     = { 1.0f, 0.0f, 0.0f, 1.0f };
vector_float4 green   = { 0.0f, 1.0f, 0.0f, 1.0f };
vector_float4 magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

@implementation GraphicsViewDelegate
{
    dispatch_semaphore_t _frameBoundarySemaphore;
    uint32_t             _currentFrameIndex;
}

-(void)configureMetal
{
    _frameBoundarySemaphore = dispatch_semaphore_create(kMaxBuffers);
    _currentFrameIndex      = 0;
}

-(void)mtkView:(MTKView *) view drawableSizeWillChange:(CGSize) size  {}

-(void)setXOffset:(float)amount {}

-(void)drawInMTKView:(MTKView *) view
{
    //Read about this in the Metal book
    dispatch_semaphore_wait(_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    //Current frame in the triple buffering I am guessing
    uint32_t frameIndex   = _currentFrameIndex;

    uint32_t vertexCount  = 0;

    game_vertex *vertices = self.gameRenderCommands.vertexBuffer[frameIndex]->vertices;

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
    
    @autoreleasepool{
        
        //Setting up the command buffer
        id<MTLCommandBuffer> CommandBuffer = [self.commandQueue commandBuffer];

        //Setting up the render pass descriptor
        MTLRenderPassDescriptor *RenderPassDescriptor       = [view currentRenderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        RenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.1f, 0.1f, 0.1f, 1.0f);
        
        //Render command encoder
        id<MTLRenderCommandEncoder> RenderEncoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPassDescriptor];
        RenderEncoder.label = @"RenderEncoder";
        
        //=======

        // 1 Viewport using actual drawable size
        CGSize drawableSize = view.drawableSize;
        MTLViewport viewPort = { 0.0, 0.0, drawableSize.width, drawableSize.height, 0.0, 1.0 };
        [RenderEncoder setViewport: viewPort];
        
        //2 Setting the pipeline state fpr the draw
        [RenderEncoder setRenderPipelineState: [self pipelineState]];
        [RenderEncoder setVertexBuffer: [[self macVertexBuffers] objectAtIndex: _currentFrameIndex]
                                offset: 0
                                 atIndex: 0];

        // PASS UNIFORMS TO FRAGMENT SHADER AT INDEX 11
        _uniforms.audioData     = _audioRenderer->data.outputData;
        _uniforms.viewportSize = (vector_float2){ 512.0f, 512.0f };

        //Bottom Left Square
        _uniforms.materialId = MAT_BOTTOM_LEFT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareAStart
                          vertexCount:squareACount];

        //Bottom right square
        _uniforms.materialId = MAT_BOTTOM_RIGHT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareBStart
                          vertexCount:squareBCount];

        //Top left
        _uniforms.materialId = MAT_TOP_LEFT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareCStart
                          vertexCount:squareCCount];
        //Top right
        _uniforms.materialId = MAT_TOP_RIGHT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareDStart
                          vertexCount:squareDCount];
        
        //=========
        
        //Finish encoding
        [RenderEncoder endEncoding];
        
        id<CAMetalDrawable> NextDrawable = [view currentDrawable];
        [CommandBuffer presentDrawable:NextDrawable];

        //Frame and looping back
        uint32_t nextIndex = _currentFrameIndex + 1;
        if (nextIndex > 2) {
            nextIndex = 0;
        }

        _currentFrameIndex = nextIndex;

        __block dispatch_semaphore_t semaphore = _frameBoundarySemaphore;

        [CommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
            dispatch_semaphore_signal(semaphore);
        }];

        //Ready for the GPU to submit
        [CommandBuffer commit];
    }
}

- (void)setBottomLeftClicked:(BOOL)clicked  {
    
    //Check bottom left square for click
    _uniforms.clickedSquare.bottomLeft  = clicked;
    //Turn on the audio
    _audioRenderer->onOff               = true;
    _audioRenderer->osc.setFrequency(349.23);
    
}
-(void)setBottomRightClicked:(BOOL)clicked
{
    _uniforms.clickedSquare.bottomRight = clicked;
    _audioRenderer->onOff               = true;
    _audioRenderer->osc.setFrequency(261.63);
}
-(void)setTopLeftClicked:(BOOL)clicked
{
    _uniforms.clickedSquare.topLeft     = clicked;
    _audioRenderer->onOff               = true;
    _audioRenderer->osc.setFrequency(392.00);
}
-(void)setTopRightClicked:(BOOL)clicked
{
    _uniforms.clickedSquare.topRight    = clicked;
    _audioRenderer->onOff               = true;
    _audioRenderer->osc.setFrequency(440.00);
}

@end
