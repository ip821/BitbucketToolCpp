#if defined(__WXOSX__)

#import <AppKit/AppKit.h>

extern "C" int GetSystemMenuBarHeight()
{
    return static_cast<int>([[NSStatusBar systemStatusBar] thickness]);
}

#endif