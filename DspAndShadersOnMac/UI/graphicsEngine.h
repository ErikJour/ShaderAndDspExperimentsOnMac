//
// Created by Erik Jourgensen on 9/22/26.
//
#pragma once
#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>
#import "Common.h"
#include "../Audio/AudioTypes.h"
#import "../Audio/AudioRenderer.h"

//Erik Note -> retain is often for Obj-C objects, assign is for simple pointers, UInt, etc
@interface
GraphicsViewDelegate: NSObject <MTKViewDelegate>
@property (retain) id<MTLCommandQueue>        commandQueue;
@property (retain) id<MTLRenderPipelineState> pipelineState;
@property GameRenderCommands                  gameRenderCommands;
@property Uniforms                            uniforms;
@property (nonatomic, assign) MyRenderer      *audioRenderer;
@property UInt32                              x;
//What is a mutable array?
@property (retain) NSMutableArray*            macVertexBuffers;

- (void)configureMetal;
- (void)setXOffset:(float)amount;
- (void)setBottomLeftClicked:(BOOL)clicked;
- (void)setBottomRightClicked:(BOOL)clicked;
- (void)setTopLeftClicked:(BOOL)clicked;
- (void)setTopRightClicked:(BOOL)clicked;

@end

