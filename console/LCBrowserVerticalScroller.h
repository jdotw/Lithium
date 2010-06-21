//
//  LCBrowserScroller.h
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBrowserVerticalScroller : NSScroller 
{
	NSColor *backgroundTintColor;
	BOOL lightenWhenNotMainWindow;
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

@property (nonatomic,copy) NSColor *backgroundTintColor;
@property (nonatomic, assign) BOOL lightenWhenNotMainWindow;

@end
