//
//  LCViewCell.m
//  Lithium Console
//
//  Created by James Wilson on 21/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCViewCell.h"


@implementation LCViewCell

#pragma Initialisation 

- (void) release
{
	[super release];
}

- (void) dealloc
{
	subView = nil;
	[super dealloc];
}

#pragma View Manipulation

- (NSView *) subView
{ return subView; }

- (void) setSubView:(NSView *)view
{ subView = view; }

#pragma Drawing Methods

- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView
{
    [[self subView] setFrame: cellFrame];
	
    if ([[self subView] superview] != controlView)
    {
		[controlView addSubview: [self subView]];
    }

	/* Draw separator */
//	NSRect sepRect = NSMakeRect(NSMinX(cellFrame), NSMaxY(cellFrame)+1, NSWidth(cellFrame), 0.5);
//	NSBezierPath *sepPath = [NSBezierPath bezierPathWithRect:sepRect];
//	[[NSColor colorWithCalibratedRed:0.2 green:0.2 blue:0.2 alpha:0.8] setFill];
//	[sepPath fill];
}

@end
