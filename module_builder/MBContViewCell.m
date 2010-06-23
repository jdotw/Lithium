//
//  MBContViewCell.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBContViewCell.h"


@implementation MBContViewCell

#pragma mark Initialisation 

- (void) release
{
	[super release];
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark View Manipulation

@synthesize subView;
@synthesize prefsView;
@synthesize customView;

#pragma mark Drawing Methods

- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView
{
	/* Set View Frames */
	[[self subView] setFrame:NSMakeRect(NSMinX(cellFrame)+3.0, NSMinY(cellFrame), 
										NSWidth(cellFrame)-3.0, NSHeight([[self subView] frame]))];
	if (prefsView)
	{
		[prefsView setFrame:NSMakeRect(NSMinX(cellFrame)+3.0, NSMinY(cellFrame) + NSHeight([[self subView] frame]),
									   NSWidth(cellFrame)-3.0, NSHeight([prefsView frame]))];
	}
	if (customView)
	{
		[customView setFrame:NSMakeRect(NSMinX(cellFrame)+3.0, NSMaxY(cellFrame) - NSHeight([customView frame]),
									   NSWidth(cellFrame)-3.0, NSHeight([customView frame]))];
	}

	/* Draw Outline */
	NSRect outlineRect = NSMakeRect(NSMinX(cellFrame) + 6.0, NSMinY(cellFrame) + 1.0, 
									NSWidth(cellFrame) - 7.0, NSHeight(cellFrame) - 2.0);
	NSBezierPath *bezierpath = [NSBezierPath bezierPathWithRoundedRect:outlineRect xRadius:5.0 yRadius:5.0];
	CGFloat alpha;
	if ([[controlView window] isMainWindow])
	{ 
		if (selected)
		{ alpha = 0.6; }
		else
		{ alpha = 1.0; }
	}
	else
	{ alpha = 0.4; }
	NSGradient *gradient = [[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:220.0/256.0 alpha:alpha]
														 endingColor:[NSColor colorWithCalibratedWhite:195.0/256.0 alpha:alpha]];
	[gradient drawInBezierPath:bezierpath angle:90.0];

	/* Add to superviews */
    if ([[self subView] superview] != controlView)
    {
		[controlView addSubview: [self subView]];
    }
    if (prefsView && [prefsView superview] != controlView)
    {
		[controlView addSubview:prefsView];
    }
    if (customView && [customView superview] != controlView)
    {
		[controlView addSubview:customView];
    }
}

#pragma mark Accessors

- (int) level
{ return level; }

- (void) setLevel:(int)num
{ level = num; }

@synthesize selected;

@end
