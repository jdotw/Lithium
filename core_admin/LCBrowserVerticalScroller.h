//
//  LCBrowserScroller.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCBrowserVerticalScroller : NSScroller 
{
	NSColor *backgroundTintColor;
}

#pragma mark Drawing
- (void) drawRect:(NSRect)rect;
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


@end
