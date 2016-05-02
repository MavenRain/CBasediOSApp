//
//  Main.cpp
//  CBasediOSApp
//
//  Created by SolomonRain on 5/1/16.
//  Copyright © 2016 Ultimate Computer Services, Inc. All rights reserved.
//

#ifdef __cplusplus
extern "C"
{
#endif

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


// this is the entry point of the application, same as -application:didFinishLaunchingWithOptions:
// note the fact that we use `void *` for the 'application' and 'options' fields, as we need no reference to them for this to work. A generic id would suffice here as well.
BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, void *application, void *options)
{
    // we +alloc and -initWithFrame: our window here, so that we can have it show on screen (eventually).
    // this entire method is the objc-runtime based version of the standard View-Based application's launch code, so nothing here really should surprise you.
    // one thing important to note, though is that we use `sel_getUid()` instead of @selector().
    // this is because @selector is an objc language construct, and the application would not have been created in C if I used @selector.
    self->window = objc_msgSend((id)objc_getClass("UIWindow"), sel_getUid("alloc"));
    self->window = objc_msgSend(self->window, sel_getUid("initWithFrame:"), (struct CGRect) { 0, 0, 0, 0 });
    
    CGRect bounds;
    object_getInstanceVariable((id)objc_getClass("UIScreen"), "mainScreen", (void * *)&bounds);
    objc_msgSend(self->window, sel_getUid("setFrame:"), bounds);
    
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

// This is a simple -drawRect implementation for our class. We could have
// used a UILabel  or something of that sort instead, but I felt that this
// stuck with the C-based mentality of the application.
void View_drawRect(id self, SEL _cmd, CGRect rect)
{
    // We are simply getting the graphics context of the current view,
    // so we can draw to it
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    // Then we set it's fill color to white so that we clear the background.
    // Note the cast to (CGFloat []). Otherwise, this would give a warning
    //  saying "invalid cast from type 'int' to 'CGFloat *', or
    // 'extra elements in initializer'. Also note the assumption of RGBA.
    // If this wasn't a demo application, I would strongly recommend against this,
    // but for the most part you can be pretty sure that this is a safe move
    // in an iOS application.
    CGContextSetFillColor(context, (CGFloat []){ 1, 1, 1, 1 });
    
    // here, we simply add and draw the rect to the screen
    CGContextAddRect(context, (struct CGRect) { 0, 0, 320, 480 });
    CGContextFillPath(context);
    
    // and we now set the drawing color to red, then add another rectangle
    // and draw to the screen
    CGContextSetFillColor(context, (CGFloat []) { 1, 0, 0, 1 });
    CGContextAddRect(context, (struct CGRect) { 10, 10, 20, 20 });
    CGContextFillPath(context);
}


#ifdef __cplusplus
}
#endif






































































