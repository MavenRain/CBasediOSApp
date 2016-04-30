//
//  AppDelegate.m
//  CBasediOSApp
//
//  Created by Richard Ross on 4/23/12.
//  Copyright (c) 2012 Ultimate Computer Services, Inc. All rights reserved.
//
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>

struct AppDel
{
    Class isa;
    
    id window;
};

// This is a strong reference to the class of the AppDelegate
// (same as [AppDelegate class])
Class AppDelClass;

// this is the entry point of the application, same as -application:didFinishLaunchingWithOptions:
// note the fact that we use `void *` for the 'application' and 'options' fields, as we need no reference to them for this to work. A generic id would suffice here as well.
BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, void *application, void *options)
{
    // we +alloc and -initWithFrame: our window here, so that we can have it show on screen (eventually).
    // this entire method is the objc-runtime based version of the standard View-Based application's launch code, so nothing here really should surprise you.
    // one thing important to note, though is that we use `sel_getUid()` instead of @selector().
    // this is because @selector is an objc language construct, and the application would not have been created in C if I used @selector.
    self->window = objc_msgSend((id)objc_getClass("UIWindow"), sel_getUid("alloc"));
    self->window = objc_msgSend(self->window, sel_getUid("initWithFrame:"), (struct CGRect) { 0, 0, 320, 480 });
    
    // here, we are creating our view controller, and our view. note the use of objc_getClass, because we cannot reference UIViewController directly in C.
    id viewController = objc_msgSend(objc_msgSend((id)objc_getClass("UIViewController"), sel_getUid("alloc")), sel_getUid("init"));
    
    // creating our custom view class, there really isn't too much 
    // to say here other than we are hard-coding the screen's bounds, 
    // because returning a struct from a `objc_msgSend()` (via 
    // [[UIScreen mainScreen] bounds]) requires a different function call
    // and is finicky at best.
    id view = objc_msgSend(objc_msgSend((id)objc_getClass("View"), sel_getUid("alloc")), sel_getUid("initWithFrame:"), (struct CGRect) { 0, 0, 320, 480 });
    
    // here we simply add the view to the view controller, and add the viewController to the window.
    objc_msgSend(objc_msgSend(viewController, sel_getUid("view")), sel_getUid("addSubview:"), view);
    objc_msgSend(self->window, sel_getUid("setRootViewController:"), viewController);
    
    // finally, we display the window on-screen.
    objc_msgSend(self->window, sel_getUid("makeKeyAndVisible"));
    
    return YES;
}


























