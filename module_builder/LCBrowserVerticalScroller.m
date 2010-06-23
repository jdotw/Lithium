//
//  LCBrowserScroller.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserVerticalScroller.h"


@implementation LCBrowserVerticalScroller

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
		frame = NSMakeRect(rect.origin.x+2,rect.origin.y+2,rect.size.width-4,rect.size.height-4);
	}

	/* Create path */
	NSBezierPath *outline = [self roundedPathInFrame:frame];
	
	/* Stroke and Fill */
	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.2] setFill];
	[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
	[outline fill];
	[outline stroke];
	
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
	if ([self usableParts] < 1) return;	
	
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
	float width = frame.size.width;
	float arcWidth = width / 2.0;
	NSBezierPath *path = [NSBezierPath bezierPath];

	/* right line */
	[path moveToPoint:NSMakePoint(NSMaxX(frame),NSMinY(frame) + arcWidth)];
	[path lineToPoint:NSMakePoint(NSMaxX(frame),NSMaxY(frame) - arcWidth)];		

	/* Top curve */
	[path appendBezierPathWithArcWithCenter:NSMakePoint(NSMinX(frame) + arcWidth,NSMaxY(frame) - arcWidth) 
									 radius:width/2.0 
								 startAngle:0 
								   endAngle:180
								  clockwise:NO];		
	
	/* left line */
	[path lineToPoint:NSMakePoint(NSMinX(frame),NSMinY(frame) + arcWidth)];
	
	/* Bottom curve */
	[path appendBezierPathWithArcWithCenter:NSMakePoint(NSMinX(frame) + arcWidth,NSMinY(frame) + arcWidth) 
									 radius:width/2.0 
								 startAngle:180
								   endAngle:0
								  clockwise:NO];
		
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
