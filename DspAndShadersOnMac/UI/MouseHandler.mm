#import <iostream>
#import "MouseHandler.h"
#import "AnimatedRenderer.hpp"

//static uint32 X          = 0;
//Just for Erik reference -> Uint32_t is a gaurnateed 32-bit (4-byte) unsigned integer, basically force non-negatives
static UInt32 clickCount = 0;

@implementation MouseHandler

- (BOOL)acceptsFirstResponder { return YES; }

- (void)raycastAtX:(uint32_t)clickedX y:(uint32_t)clickedY minX:(uint32_t)minX maxX:(uint32_t)maxX minY:(uint32_t)minY maxY:(uint32_t)maxY  callback:(void (^)(bool))fn
{
    //Basic hit testing
    if (clickedX >= minX && clickedX <= maxX  && clickedY >= minY && clickedY <= maxY) {

        clickCount++;
        fn(true);
    }
}

- (void) mouseDown:(NSEvent *) event
{
    //Mouse hit locations
    double clickedX = event.locationInWindow.x;
    double clickedY = event.locationInWindow.y;
    _renderer->MouseDown(clickedX, clickedY);
    
//    //Crude raycasting system
//    [self raycastAtX:   clickedX
//                        y:clickedY
//                        minX:0
//                        maxX:GLOBAL_WIDTH / 2
//                        minY:0
//                        maxY:GLOBAL_HEIGHT /2
//                        callback:^(bool clicked) {[self->_viewDelegate setBottomLeftClicked:clicked]; }];
//    
//    [self raycastAtX:   clickedX
//                        y:clickedY
//                        minX:GLOBAL_WIDTH / 2
//                        maxX:GLOBAL_WIDTH
//                        minY:0
//                        maxY:GLOBAL_HEIGHT /2
//                        callback:^(bool clicked) {[self->_viewDelegate setBottomRightClicked:clicked]; }];
//    
//    [self raycastAtX:   clickedX
//                        y:clickedY
//                        minX:0
//                        maxX:GLOBAL_WIDTH / 2
//                        minY:GLOBAL_HEIGHT /2
//                        maxY:GLOBAL_HEIGHT
//                        callback:^(bool clicked) {[self->_viewDelegate setTopLeftClicked:clicked]; }];
//    
//    [self raycastAtX:   clickedX
//                        y:clickedY
//                        minX:GLOBAL_WIDTH / 2
//                        maxX:GLOBAL_WIDTH
//                        minY:GLOBAL_HEIGHT /2
//                        maxY:GLOBAL_HEIGHT
//                        callback:^(bool clicked) {[self->_viewDelegate setTopRightClicked:clicked]; }];
}

- (void) mouseUp:(NSEvent *) event {
    double clickedX = event.locationInWindow.x;
    double clickedY = event.locationInWindow.y;
    _renderer->MouseUp(clickedX, clickedY);
//    [_viewDelegate setXOffset:10];
//    [_viewDelegate setBottomLeftClicked:false];
//    [_viewDelegate setBottomRightClicked:false];
//    [_viewDelegate setTopLeftClicked:false];
//    [_viewDelegate setTopRightClicked:false];
//    _viewDelegate.audioRenderer->onOff = false;
}

- (void)mouseMoved:(NSEvent*)event   { std::cout << event.locationInWindow.x << std::endl; }
- (void)scrollWheel:(NSEvent *)event { std::cout << event.scrollingDeltaY << std::endl; }

@end
