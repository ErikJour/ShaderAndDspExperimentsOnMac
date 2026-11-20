
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <iostream>
#import "graphicsEngine.h"
#import "Common.h"
#import "graphicsView.h"


static uint32 X          = 0;
static UInt32 clickCount = 0;

MTK::View* GraphicsMtkView::initWithFrame(CGRect frame, MyRenderer* audioRenderer)
{
    //Device setup
    MTL::Device* mDevice = MTLCreateSystemDefaultDevice();
    self                 = [super initWithFrame:frame device:device];
    if (!self) return nil;

    self.framebufferOnly = YES;
    CGColorSpaceRef cs   = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    self.colorspace      = cs;
    self.clearColor      = MTLClearColorMake(0.0, 0.0, 1.0, 1.0);
    CGColorSpaceRelease(cs);

    _commandQueue = [device newCommandQueue];

    //============================================================================
    // Shader Library & Render Pipeline Setup
    //============================================================================
    NSError *error     = nil;
    NSString *libPath  = [NSBundle.mainBundle.resourcePath
                         stringByAppendingPathComponent:@"default.metallib"];
    id<MTLLibrary> lib = [device newLibraryWithURL:[NSURL fileURLWithPath:libPath] error:&error];
    if (!lib) { NSLog(@"Failed to load metallib at path %@: %@", libPath, error); return nil; }

    //Setting up shader function names and logging error
    id<MTLFunction> vfn = [lib newFunctionWithName:@"vertexMain"];
    id<MTLFunction> ffn = [lib newFunctionWithName:@"fragmentMain"];
    NSAssert(vfn && ffn, @"Missing shader functions");

    //Render pipeline descriptor
    MTLRenderPipelineDescriptor *desc    = [MTLRenderPipelineDescriptor new];
    desc.vertexFunction                  = vfn;
    desc.fragmentFunction                = ffn;
    desc.colorAttachments[0].pixelFormat = self.colorPixelFormat;
    _solidColorPipelineState             = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!_solidColorPipelineState)         { NSLog(@"Pipeline error: %@", error); return nil; }

    //============================================================================
    // Buffer Setup
    //============================================================================
    NSUInteger page             = getpagesize();
    NSUInteger raw              = (NSUInteger)GLOBAL_WIDTH * GLOBAL_HEIGHT * sizeof(game_vertex);
    NSUInteger vertexBufferSize = (raw + page - 1) & ~(page - 1);

    GameRenderCommands gameRenderCommand = {};
    NSMutableArray *macVertexBuffers = [NSMutableArray arrayWithCapacity:kMaxBuffers];

    //Triple buffering
    for (int i = 0; i < kMaxBuffers; i++) {
        
        //We allocate a pointer to hold a buffer
        auto *buf     = (VertexBuffer *)malloc(sizeof(VertexBuffer));
        
        //The buffer's position and color can be stored in here
        buf->vertices = (game_vertex *)mmap(nullptr, vertexBufferSize,
                                            PROT_READ | PROT_WRITE,
                                            MAP_PRIVATE | MAP_ANON, -1, 0);
        
        //Put this into a game render command
        gameRenderCommand.vertexBuffer[i] = buf;

        //We create a metal buffer that takes in position and color, is the size in bytes of pages (I think) multipled by aspect ratio
        id<MTLBuffer> metalBuffer         = [device newBufferWithBytesNoCopy:buf->vertices
                                                    length:vertexBufferSize
                                                    options:MTLResourceStorageModeShared
                                                    deallocator:^(void *p, NSUInteger len) { munmap(p, len); }];
        
        //Add the metal buffer to the macVertexBuffers array
        [macVertexBuffers addObject:metalBuffer];
    }

    //============================================================================
    // Delegate Setup
    //============================================================================
    _viewDelegate                    = [[GraphicsViewDelegate alloc] init];
    _viewDelegate.commandQueue       = _commandQueue;
    _viewDelegate.pipelineState      = _solidColorPipelineState;
    _viewDelegate.macVertexBuffers   = macVertexBuffers;
    _viewDelegate.gameRenderCommands = gameRenderCommand;
    _viewDelegate.audioRenderer      = audioRenderer;
    _viewDelegate.x                  = X;
    [_viewDelegate configureMetal];
    self.delegate                    = _viewDelegate;

    return self;
}

bool GraphicsMtkView::acceptsFirstResponder() { return YES; }

void GraphicsMtkView::raycastAtX(uint32_t x,
                uint32_t y,
                uint32_t minX,
                uint32_t maxX,
                uint32_t minY,
                uint32_t maxY,
                std::function<void(bool)> callback))
{
    //Basic hit testing
    if (clickedX >= minX && clickedX <= maxX  && clickedY >= minY && clickedY <= maxY) {

        clickCount++;
        fn(true);
        
    }
}

void GraphicsMtkView::mouseDown(NSEvent* event)
{
    //Mouse hit locations
    double clickedX = event.locationInWindow.x;
    double clickedY = event.locationInWindow.y;
    
    //Crude raycasting system
    [self raycastAtX:   clickedX
                        y:clickedY
                        minX:0
                        maxX:GLOBAL_WIDTH / 2
                        minY:0
                        maxY:GLOBAL_HEIGHT /2
                        callback:^(bool clicked) {[self->_viewDelegate setBottomLeftClicked:clicked]; }];
    
    [self raycastAtX:   clickedX
                        y:clickedY
                        minX:GLOBAL_WIDTH / 2
                        maxX:GLOBAL_WIDTH 
                        minY:0
                        maxY:GLOBAL_HEIGHT /2
                        callback:^(bool clicked) {[self->_viewDelegate setBottomRightClicked:clicked]; }];
    
    [self raycastAtX:   clickedX
                        y:clickedY
                        minX:0
                        maxX:GLOBAL_WIDTH / 2
                        minY:GLOBAL_HEIGHT /2
                        maxY:GLOBAL_HEIGHT
                        callback:^(bool clicked) {[self->_viewDelegate setTopLeftClicked:clicked]; }];
    
    [self raycastAtX:   clickedX
                        y:clickedY
                        minX:GLOBAL_WIDTH / 2
                        maxX:GLOBAL_WIDTH
                        minY:GLOBAL_HEIGHT /2
                        maxY:GLOBAL_HEIGHT
                        callback:^(bool clicked) {[self->_viewDelegate setTopRightClicked:clicked]; }];
}

void GraphicsMtkView::mouseUp(NSEvent* event) {
    [_viewDelegate setXOffset:10];
    [_viewDelegate setBottomLeftClicked:false];
    [_viewDelegate setBottomRightClicked:false];
    [_viewDelegate setTopLeftClicked:false];
    [_viewDelegate setTopRightClicked:false];
    _viewDelegate.audioRenderer->onOff = false;
}

- (void)mouseMoved:(NSEvent*)event   { std::cout << event.locationInWindow.x << std::endl; }
- (void)scrollWheel:(NSEvent *)event { std::cout << event.scrollingDeltaY << std::endl; }

@end



