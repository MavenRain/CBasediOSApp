//
//  ViewController.c
//  CBasediOSApp
//
//  Created by Richard Ross on 4/23/12.
//  Copyright (c) 2012 Ultimate Computer Services, Inc. All rights reserved.
//
#include <objc/runtime.h>
#include <CoreGraphics/CoreGraphics.h>

// This is a strong reference to the class of our custom view,
// In case we need it in the future.
Class ViewClass;

// Notice this. We must create this as an extern function, as we cannot include all
// of UIKit. This works, but is definitely not optimal.
extern CGContextRef UIGraphicsGetCurrentContext();

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