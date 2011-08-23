//
//  LCBrowserTableHeaderCell.m
//  Lithium Console
//
//  Created by James Wilson on 7/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTableHeaderCell.h"
#import "LCTriangleBezierPath.h"

@implementation LCBrowserTableHeaderCell 

- (LCBrowserTableHeaderCell *) init
{
	[super init];
	[self setTextColor:[NSColor whiteColor]];
	return self;
}

- (void)drawInteriorWithFrame:(NSRect)rect inView:(NSView *)controlView
{
	NSImage *backImage = [NSImage imageNamed:@"tableheaderback.png"];
	[backImage setFlipped:YES];
	[backImage drawInRect:cellFrame
				 fromRect:NSMakeRect(0,0,[backImage size].width,[backImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
	/* Text */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
						  [NSColor colorWithCalibratedWhite:1.0 alpha:0.15], NSForegroundColorAttributeName,
						  [NSFont fontWithName:@"Arial Bold" size:10.0], NSFontAttributeName,
						  nil];
	NSRect textRect = NSMakeRect(cellFrame.origin.x+3, cellFrame.origin.y+2, cellFrame.size.width-3, cellFrame.size.height);
	[[self stringValue] drawInRect:textRect withAttributes:attr];
	
	attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedWhite:0.10 alpha:1.0], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Arial Bold" size:10.0], NSFontAttributeName,
			nil];
	textRect = NSMakeRect(cellFrame.origin.x+3, cellFrame.origin.y+1, cellFrame.size.width-3, cellFrame.size.height);
	[[self stringValue] drawInRect:textRect withAttributes:attr];
	
	/* Resize bar */
	NSRect barRect = NSMakeRect(NSMaxX(rect)-1,NSMinY(rect),1,NSHeight(rect));
	NSBezierPath *barPath = [NSBezierPath bezierPathWithRect:barRect];
	[[NSColor colorWithCalibratedWhite:0.6 alpha:0.12] setFill];
	[barPath fill];
	
	/* Non-Key Lighten */
	if (![[controlView window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.6 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:cellFrame] fill];
	}	
}

- (void)drawWithFrame:(NSRect)rect inView:(NSView *)controlView
{
	cellFrame = rect;
	[super drawWithFrame:rect inView:controlView];
}

- (void)drawSortIndicatorWithFrame:(NSRect)rect inView:(NSView *)controlView ascending:(BOOL)ascending priority:(NSInteger)priority
{
	if (priority == 0)
	{
		NSRect indicatorRect = [self sortIndicatorRectForBounds:cellFrame];
		NSBezierPath *path;
		if (ascending) path = [LCTriangleBezierPath upTriangleInRect:indicatorRect];
		else path = [LCTriangleBezierPath downTriangleInRect:indicatorRect];
		[[NSColor colorWithCalibratedWhite:1.0 alpha:0.4] setFill];
		[path fill];
	}
}

- (void)highlight:(BOOL)flag withFrame:(NSRect)rect inView:(NSView *)controlView
{
	[super highlight:flag withFrame:rect inView:controlView];
}

@end
