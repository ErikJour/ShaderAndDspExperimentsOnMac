//
//  ApplicationDelegate.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/18/26.
//
#pragma once
#include "AnimatedView.hpp"
#include "AppKit/AppKit.hpp"
#include <MetalKit/MetalKit.hpp>
#include "Common.h"
#include "../Audio/AudioRenderer.h"

class AppDelegate : public NS::ApplicationDelegate
{
public:
    
    AppDelegate() = default;
    ~AppDelegate();
    
    void applicationDidFinishLaunching(NS::Notification* notification);
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender);
    
    //===============
    std::string      title{"Shaders and DSP"};
    NS::Window*      mWindow = nullptr;
    MTK::View*       mView   = nullptr; //replace this with mouse handler=
    MTL::Device*     mDevice = nullptr;
    AnimatedView*    mAppView = nullptr;
    MyRenderer       mAudioRenderer;
};
