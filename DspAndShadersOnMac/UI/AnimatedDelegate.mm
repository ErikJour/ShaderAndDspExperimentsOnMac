//
//  AppDel.cpp
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/18/26.
//
#include "AnimatedDelegate.h"
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
    
    CGRect screenRect = CGDisplayBounds(CGMainDisplayID());
    CGRect windowRect =  {
                        { (screenRect.size.width - GLOBAL_WIDTH) * 0.5,
                            (screenRect.size.height - GLOBAL_HEIGHT) * 0.5 },
                        { GLOBAL_WIDTH, GLOBAL_HEIGHT }
    };
    
    mWindow = NS::Window::alloc()->init (windowRect,
                                       NS::WindowStyleMaskTitled |
                                       NS::WindowStyleMaskClosable |
                                       NS::WindowStyleMaskMiniaturizable,
                                       NS::BackingStoreBuffered,
                                       false);
    
    
    MouseHandler* _mouseHandler;
    
    _mouseHandler = [[MouseHandler alloc] initWithFrame:windowRect device:(id<MTLDevice>)mDevice];
    
    mView = (MTK::View*)_mouseHandler;

    mView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    
    mView->setClearColor(MTL::ClearColor::Make( 0.0,
                                                1.0,
                                                0.0,
                                                1.0));
    
    mAppView = new AnimatedView(mDevice,
                                &mAudioRenderer);
    
    mView->setDelegate(mAppView);
    _mouseHandler.renderer = mAppView->getRenderer();

    mWindow->setContentView(mView);
    
    mWindow->setTitle(NS::String::string(title.c_str(),
                      NS::StringEncoding::UTF8StringEncoding));

    mWindow->makeKeyAndOrderFront(nullptr);
    
    auto* appNS = reinterpret_cast<NS::Application*>(notification->object());
    
    appNS->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender)
{
    stopAudio(&mAudioRenderer);

    return TRUE;
}

