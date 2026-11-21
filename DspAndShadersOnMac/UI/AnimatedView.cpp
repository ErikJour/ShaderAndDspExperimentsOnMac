//
//  AnimatedView.cpp
//  DspAndShadersOnMacApp
//
//  Created by Erik Jourgensen on 11/20/26.
//
#include "AnimatedView.hpp"


AnimatedView::AnimatedView(MTL::Device* device, MyRenderer* audioRenderer) : renderer{new AnimatedRenderer(device, audioRenderer)} {}
    
AnimatedView::~AnimatedView()
{
    delete renderer;
}
    
void AnimatedView::drawInMTKView(MTK::View *pView) {
        renderer->Draw(pView);
    };

    


