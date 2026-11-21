//
//  graphicsView.h
//  DspAndShadersOnMacApp
//
//  Created by Erik Jourgensen on 11/21/26.
//

//
//  graphicsView.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/2/26.
//
#pragma once
#import <MetalKit/MetalKit.h>
#import "AnimatedRenderer.hpp"

@interface MouseHandler : MTKView

//Hit testing
- (void)raycastAtX:(uint32_t)clickedX y:(uint32_t)clickedY minX:(uint32_t)minX maxX:(uint32_t)maxX minY:(uint32_t)minY maxY:(uint32_t)maxY callback:(void (^)(bool))fn;

@end
