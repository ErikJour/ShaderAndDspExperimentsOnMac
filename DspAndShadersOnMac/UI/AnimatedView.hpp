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
#include "../Audio/AudioRenderer.h"


class AnimatedView : public MTK::ViewDelegate
{
public:
    explicit AnimatedView(MTL::Device* device, MyRenderer* audioRenderer);
    
    ~AnimatedView() override;
    
    void drawInMTKView(MTK::View *pView) override;
    
    AnimatedRenderer* getRenderer() { return renderer; }
    
private:
    AnimatedRenderer* renderer{nullptr};
    
};

#endif /* AnimatedView_hpp */
