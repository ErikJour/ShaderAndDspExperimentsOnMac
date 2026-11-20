//
//  graphicsView.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/2/26.
//
#pragma once
#include <MetalKit/MetalKit.hpp>
#import "../Audio/AudioRenderer.h"   // for MyRenderer

class GraphicsMtkView : public MTK::View
{
public:
    GraphicsMtkView();
    ~GraphicsMtkView();
    MTK::View* initWithFrame(CGRect frame, MyRenderer* audioRenderer);
    void raycastAtX(uint32_t x,
                    uint32_t y,
                    uint32_t minX,
                    uint32_t maxX,
                    uint32_t minY,
                    uint32_t maxY,
                    std::function<void(bool)> callback));
private:
    GraphicsViewDelegate*       mViewDelegate;
    id<MTLCommandQueue>         mCommandQueue;
    id<MTLRenderPipelineState>  mSolidColorPipelineState;
}
