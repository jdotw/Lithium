//
//  LCBrowserHorizontalScroller.h
//  Lithium Console
//
//  Created by James Wilson on 6/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface LCBrowserHorizontalScroller : NSScroller 
{
	NSColor *backgroundTintColor;
}

#pragma mark Drawing
//- (void) drawRect:(NSRect)rect;
- (void)drawParts;
- (void)drawKnob;
- (void) drawKnobSlot;
- (void) drawButtons;
- (NSBezierPath *) roundedPathInFrame:(NSRect)frame;

#pragma mark "Triangle Path"
- (NSBezierPath *) triangleInFrame:(NSRect)frame;

#pragma mark "Misc"
- (BOOL)isOpaque;
- (NSColor *) backgroundTintColor;
- (void) setBackgroundTintColor:(NSColor *)value;


@property (retain,getter=backgroundTintColor,setter=setBackgroundTintColor:) NSColor *backgroundTintColor;

@end
