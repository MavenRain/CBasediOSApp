//
//  main.m
//  CBasediOSApp
//
//  Created by Richard Ross on 4/23/12.
//  Copyright (c) 2012 Ultimate Computer Services, Inc. All rights reserved.
//
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>

#include <objc/runtime.h>
#include <objc/message.h>

extern void UIApplicationMain(int, ...);

// This is equivalent to creating a @class with one public variable named 'window'.
struct AppDel
{
    Class isa;
    
    id window;
};

// This is a strong reference to the class of the AppDelegate
// (same as [AppDelegate class])
Class AppDelClass;

BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, void *application, void *options);

// This is a strong reference to the class of our custom view,
// In case we need it in the future.
Class ViewClass;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

void View_drawRect(id self, SEL _cmd, CGRect rect);

int main(int argc, char *argv[])
{
    // This is objc-runtime gibberish at best. We are creating a class with the
    // name "AppDelegate" that is a subclass of "UIResponder". Note we do not need
    // to register for the UIApplicationDelegate protocol, that really is simply for
    // Xcode's autocomplete, we just need to implement the method and we are golden.
    AppDelClass = objc_allocateClassPair((Class) objc_getClass("UIResponder"), "AppDelegate", 0);
    
    // Here, we tell the objc runtime that we have a variable named "window" of type 'id'
    class_addIvar(AppDelClass, "window", sizeof(id), 0, "@");
    
    // We tell the objc-runtime that we have an implementation for the method
    // -application:didFinishLaunchingWithOptions:, and link that to our custom
    // function defined above. Notice the final parameter. This tells the runtime
    // the types of arguments received by the function.
    class_addMethod(AppDelClass, sel_getUid("application:didFinishLaunchingWithOptions:"), (IMP) AppDel_didFinishLaunching, "i@:@@");
    
    // Finally we tell the runtime that we have finished describing the class and
    // we can let the rest of the application use it.
    objc_registerClassPair(AppDelClass);
    
    // Once again, just like the app delegate, we tell the runtime to
    // create a new class, this time a subclass of 'UIView' and named 'View'.
    ViewClass = objc_allocateClassPair((Class) objc_getClass("UIView"), "View", 0);
    
    // and again, we tell the runtime to add a function called -drawRect:
    // to our custom view. Note that there is an error in the type-specification
    // of this method, as I do not know the @encode sequence of 'CGRect' off
    // of the top of my head. As a result, there is a chance that the rect
    // parameter of the method may not get passed properly.
    class_addMethod(ViewClass, sel_getUid("drawRect:"), (IMP) View_drawRect, "v@:");
    
    // And again, we tell the runtime that this class is now valid to be used.
    // At this point, the application should run and display the screenshot shown below.
    objc_registerClassPair(ViewClass);
    
    id autoreleasePool = objc_msgSend(objc_msgSend((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc")), sel_registerName("init"));
    
    UIApplicationMain(argc, argv, nil, CFSTR("AppDelegate"));
    
    objc_msgSend(autoreleasePool, sel_registerName("drain"));
}












































