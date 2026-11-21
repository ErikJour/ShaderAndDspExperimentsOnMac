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
    
}

- (void) mouseUp:(NSEvent *) event {
    double clickedX = event.locationInWindow.x;
    double clickedY = event.locationInWindow.y;
    _renderer->MouseUp(clickedX, clickedY);
}

- (void)mouseMoved:(NSEvent*)event   { std::cout << event.locationInWindow.x << std::endl; }
- (void)scrollWheel:(NSEvent *)event { std::cout << event.scrollingDeltaY << std::endl; }

@end
