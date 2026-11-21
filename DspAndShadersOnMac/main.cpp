//
//  DspAndShadersOnMac
//  main.cpp
//  Created by Erik Jourgensen on 10/31/26.
//

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include "metal-cpp-extensions/AppKit/AppKit.hpp"
#include <iostream>
#include "UI/AnimatedDelegate.h"

int main()
{
    //For memory management
    NS::AutoreleasePool* poolAllocator = NS::AutoreleasePool::alloc()->init();
    
    //Create application delegate for customizing the behaviour of the application
    AppDelegate delegate;
    
    //Create system-level application object to provide main event loop
    NS::Application* appNS = NS::Application::sharedApplication();
    
    appNS->setDelegate(&delegate);
    
    appNS->run();
    
    poolAllocator->release();
    
    return 0;

}
