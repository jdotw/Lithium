//
//  LCBrowserHorizontalScroller.m
//  Lithium Console
//
//  Created by James Wilson on 6/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserHorizontalScroller.h"


@implementation LCBrowserHorizontalScroller

#pragma mark Drawing

- (void) drawRect:(NSRect)rect
{
	/* Background */
	if (backgroundTintColor)
	{
		NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
		[backgroundTintColor setFill];
		[backPath fill];
	}
	
	/*
	 * Draws the outline and fill of the control
	 */
	
	/* Get drawing frame */
	NSRect frame;
	if ([self usableParts] == 2)
	{
		/* Knob/Knob Slot will be drawn so we adjust our origin to match
		 * the knobslot, and reduce our size to fit our bounds
		 */
		rect = [self bounds];
		NSRect slotRect = [self rectForPart:NSScrollerKnobSlot];
		frame = NSMakeRect(slotRect.origin.x+2,slotRect.origin.y+2,
						   rect.size.width-(4 + (slotRect.origin.x - rect.origin.x)),
						   rect.size.height-(4 + (slotRect.origin.y - rect.origin.y)));
	}
	else
	{
		/* No knob or slot being drawn */
		rect = [self bounds];
		frame = NSMakeRect(rect.origin.x+2,rect.origin.y+6,rect.size.width-4,rect.size.height-8);
	}
	
	/* Create path */
	if ([self arrowsPosition] != NSScrollerArrowsNone)
	{
		NSBezierPath *outline = [self roundedPathInFrame:frame];
		
		/* Stroke and Fill */
		[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.2] setFill];
		[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
		[outline fill];
		[outline stroke];
	}
	
	/* Draw Parts */
	[self drawParts];	
}

- (void)drawParts
{
	[self drawButtons];
	[self drawKnobSlot];
	[self drawKnob];
}

- (void)drawKnob
{
	if ([self usableParts] != 2) return;
	NSRect knobRect = [self rectForPart:NSScrollerKnob];
	NSBezierPath *outline = [self roundedPathInFrame:NSMakeRect(knobRect.origin.x+4,knobRect.origin.y+4,knobRect.size.width-8,knobRect.size.height-8)];
	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.4] setFill];
	[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.4] setStroke];
	[outline fill];
	[outline stroke];		
}

- (void) drawKnobSlot
{
	if ([self usableParts] != 2) return;
	NSRect rect = [self rectForPart:NSScrollerKnobSlot];
	NSBezierPath *outline = [self roundedPathInFrame:NSMakeRect(rect.origin.x+2,rect.origin.y+2,rect.size.width-4,rect.size.height-4)];
	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.2] setFill];
	[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
	[outline fill];
	[outline stroke];
}

- (void) drawButtons
{
	if ([self usableParts] < 1 || [self arrowsPosition] == NSScrollerArrowsNone) return;	
	
	/* Get base rects */
	NSRect rect = [self bounds];	
	NSRect slotRect = [self rectForPart:NSScrollerKnobSlot];
	
	/* Set colour */
	[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.3] setFill];
	
	/* Up Button */
	NSRect upRect = NSMakeRect(NSMinX(rect)+5, NSMaxY(slotRect) + 4, rect.size.width-10, rect.size.width-10);
	NSBezierPath *upPath = [NSBezierPath bezierPath];
	[upPath moveToPoint:NSMakePoint(NSMinX(upRect),NSMaxY(upRect))];
	[upPath lineToPoint:NSMakePoint(NSMaxX(upRect),NSMaxY(upRect))];
	[upPath lineToPoint:NSMakePoint(NSMinX(upRect)+(upRect.size.width/2.0), NSMinY(upRect))];
	[upPath lineToPoint:NSMakePoint(NSMinX(upRect),NSMaxY(upRect))];	
	[upPath fill];
	
	/* Down Button */
	NSRect downRect = NSMakeRect(NSMinX(rect)+5, NSMaxY(slotRect) + 15, rect.size.width-10, rect.size.width-10);
	NSBezierPath *downPath = [NSBezierPath bezierPath];
	[downPath moveToPoint:NSMakePoint(NSMinX(downRect),NSMinY(downRect))];
	[downPath lineToPoint:NSMakePoint(NSMaxX(downRect),NSMinY(downRect))];
	[downPath lineToPoint:NSMakePoint(NSMinX(downRect)+(downRect.size.width/2.0), NSMaxY(downRect))];
	[downPath lineToPoint:NSMakePoint(NSMinX(downRect),NSMinY(downRect))];		
	[downPath fill];		
}

#pragma mark "Outline Path"

- (NSBezierPath *) roundedPathInFrame:(NSRect)frame
{
	float height = frame.size.height;
	float arcWidth = height / 2.0;
	NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:frame xRadius:arcWidth yRadius:arcWidth];
	return path;
}

#pragma mark "Triangle Path"

- (NSBezierPath *) triangleInFrame:(NSRect)frame
{
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(NSMinX(frame),NSMinY(frame))];
	[path lineToPoint:NSMakePoint(NSMaxX(frame),NSMinY(frame))];
	[path lineToPoint:NSMakePoint(NSMinX(frame)+(frame.size.width/2.0), NSMaxY(frame))];
	[path lineToPoint:NSMakePoint(NSMinX(frame),NSMinY(frame))];
	return path;
}

#pragma mark "Misc"

- (BOOL)isOpaque
{ return NO; }

- (NSColor *) backgroundTintColor
{ return backgroundTintColor; }

- (void) setBackgroundTintColor:(NSColor *)value
{ 
	[backgroundTintColor release];
	backgroundTintColor = [value retain];
}


@end
