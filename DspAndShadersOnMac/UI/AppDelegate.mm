//
//  AppDel.cpp
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/18/26.
//
#include "AppDelegate.h"
#include <CoreGraphics/CoreGraphics.h>
#include "MouseHandler.h"


AppDelegate::~AppDelegate() {
    mView->release();
    mWindow->release();
    mDevice->release();
    delete mAppView;
}

void AppDelegate::applicationDidFinishLaunching(NS::Notification* notification)
{
    
    renderAudio(&mAudioRenderer);
    mDevice = MTL::CreateSystemDefaultDevice();
    
    //============================================================================
    //Window Setup
    //============================================================================
    
    //Setwindow frame dimensions
    CGRect screenRect = CGDisplayBounds(CGMainDisplayID());
    CGRect windowRect =  {
        { (screenRect.size.width - GLOBAL_WIDTH) * 0.5,
            (screenRect.size.height - GLOBAL_HEIGHT) * 0.5 },
        { GLOBAL_WIDTH, GLOBAL_HEIGHT }
    };
    
    //Create window object with properties
    mWindow = NS::Window::alloc()->init (windowRect,
                                       NS::WindowStyleMaskTitled |
                                       NS::WindowStyleMaskClosable |
                                       NS::WindowStyleMaskMiniaturizable,
                                       NS::BackingStoreBuffered,
                                       false);
    
    //Set up the view, and use the custom MouseHandler
    MouseHandler* _mouseHandler;
    _mouseHandler = [[MouseHandler alloc] initWithFrame:windowRect device:(id<MTLDevice>)mDevice];
    
    mView = (MTK::View*)_mouseHandler;

    mView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    mView->setClearColor(MTL::ClearColor::Make( 0.0, 1.0, 0.0, 1.0));
    
    mAppView = new AnimatedView(mDevice, &mAudioRenderer);
    mView->setDelegate(mAppView);
    
    mWindow->setContentView(mView);
    mWindow->setTitle(NS::String::string(title.c_str(), NS::StringEncoding::UTF8StringEncoding));

    //Get global shared application state and activate it -> this will open the actual window
    mWindow->makeKeyAndOrderFront(nullptr);
    auto* appNS = reinterpret_cast<NS::Application*>(notification->object());
    appNS->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender)
{
    stopAudio(&mAudioRenderer);

    return TRUE;
}

