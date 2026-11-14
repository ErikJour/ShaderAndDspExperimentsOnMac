//
//  main.mm
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 10/31/26.
//

#include <AppKit/AppKit.h>
#include "UI/appDelegate.h"


static BtWindowDel *btWindowDelegate = nil;

int main(int argc, const char *argv[])
{
    
    NSApplication *app = [NSApplication sharedApplication];
    btWindowDelegate   = [[BtWindowDel alloc] init];
    app.delegate       = btWindowDelegate;

    return NSApplicationMain(argc, argv);
}
