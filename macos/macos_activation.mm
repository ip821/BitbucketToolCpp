//
// Created by Igor Palkin on 19.12.2025.
//

#if defined(__WXOSX__)

#import <Cocoa/Cocoa.h>

extern "C" void ShowDockIcon()
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
    });
}

extern "C" void HideDockIcon()
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
    });
}

#endif