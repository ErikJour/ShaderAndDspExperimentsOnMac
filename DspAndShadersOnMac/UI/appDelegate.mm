//
//  appDelegate.mm
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 11/6/26.
//
#import "appDelegate.h"
#import "graphicsView.h"
#import "graphicsEngine.h"
#import "../Audio/AudioRenderer.h"

@implementation BtWindowDel
{
    NSWindow*                   _window;
    GraphicsMTKView*            _metalKitView;
    MyRenderer                  _myAudioRenderer;
}

- (void)applicationDidFinishLaunching:(NSNotification *)note
{

    renderAudio(&_myAudioRenderer);

    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect((screenRect.size.width - GLOBAL_WIDTH) * 0.5,
                                   (screenRect.size.height - GLOBAL_HEIGHT) * 0.5,
                                   GLOBAL_WIDTH, GLOBAL_HEIGHT);
    //============================================================================
    //Window Setup
    //============================================================================
    _window = [[NSWindow alloc] initWithContentRect: windowRect
                                          styleMask: NSWindowStyleMaskTitled |
                                                     NSWindowStyleMaskClosable |
                                                     NSWindowStyleMaskMiniaturizable
                                            backing: NSBackingStoreBuffered
                                              defer: NO];

    _window.releasedWhenClosed    = NO;
    _window.minSize               = NSMakeSize(GLOBAL_WIDTH, GLOBAL_HEIGHT);
    _window.backgroundColor       = [NSColor blackColor];
    _window.title                 = @"Animated LPG";
    _window.delegate              = self;
    _metalKitView = [[GraphicsMTKView alloc] initWithFrame:NSMakeRect(0, 0, GLOBAL_WIDTH, GLOBAL_HEIGHT)
                                        audioRenderer:&_myAudioRenderer];
    _window.contentView           = _metalKitView;
    [_window makeFirstResponder:_metalKitView];
    //============================================================================
    //Renderer Setup
    //============================================================================
    if ([NSUserDefaults.standardUserDefaults boolForKey:@"keepOnTop"])
    {
        _window.level = NSFloatingWindowLevel;
    }

    [_window makeKeyAndOrderFront: nil];
    [NSApp activate];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    stopAudio(&_myAudioRenderer);

    return YES;
}

@end
