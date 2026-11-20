//
//  AnimatedView.hpp
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/19/26.
//

#ifndef AnimatedView_hpp
#define AnimatedView_hpp
#include <stdio.h>
#include <MetalKit/MetalKit.hpp>
#include "AnimatedRenderer.hpp"


class AnimatedView : public MTK::ViewDelegate
{
public:
    explicit AnimatedView(MTL::Device* device) : renderer{new AnimatedRenderer(device)} {}
    
    ~AnimatedView() override { delete renderer; }
    
    void drawInMTKView(MTK::View *pView) override {
        renderer->Draw(pView);
    };
    
private:
    AnimatedRenderer* renderer{nullptr};
    
};

#endif /* AnimatedView_hpp */
