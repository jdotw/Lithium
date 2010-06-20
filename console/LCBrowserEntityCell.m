//
//  LCBrowserEntityCell.m
//  Lithium Console
//
//  Created by James Wilson on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserEntityCell.h"

@implementation LCBrowserEntityCell

#pragma mark Drawing

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Create shadow */
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(1.0, -1.0)]; 
	[theShadow setShadowBlurRadius:1];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3]];

	/* Draw dot */
	NSImage *image;
	switch ([[entity opstateInteger] intValue])
	{
		case 0:
			image = [NSImage imageNamed:@"GreenDotFlipped.tiff"];
			break;
		case 1:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 2:
			image = [NSImage imageNamed:@"YellowDotFlipped.tiff"];
			break;
		case 3:
			image = [NSImage imageNamed:@"RedDotFlipped.tiff"];
			break;
		default:
			image = [NSImage imageNamed:@"GreyDotFlipped.tiff"];	
	}
	if (![entity opstateInteger])
	{ image = nil; }
	if (image)
	{
		[image drawInRect:NSMakeRect(cellFrame.origin.x, cellFrame.origin.y+(0.18 * cellFrame.size.height), cellFrame.size.height * 0.7, cellFrame.size.height * 0.7)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	}
	
	
	/* Set string attributes */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.02 green:0.02 blue:0.02 alpha:1.0], NSForegroundColorAttributeName,
		[self font], NSFontAttributeName,
		nil];

	/* Draw string */
	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	[[self stringValue] drawAtPoint:NSMakePoint(cellFrame.origin.x+14, cellFrame.origin.y + (0.25 * textSize.height)) withAttributes:attr];
	
	/* Draw Refresh overlay */
	if ([entity refreshInProgress])
	{
		NSImage *refImage = [NSImage imageNamed:@"refresh_grey_16.tif"];
		[refImage drawInRect:NSMakeRect(NSMaxX(cellFrame)-18, NSMinY(cellFrame), NSHeight(cellFrame), NSHeight(cellFrame))
					fromRect:NSMakeRect(0, 0, [refImage size].width, [refImage size].height)
				   operation:NSCompositeSourceOver
					fraction:1.0];
	}
	
	[theShadow release];
}

- (void) dealloc
{
	[self unbind:@"opStateInteger"];
	[super dealloc];
}

#pragma mark Accessors
@synthesize entity;

@end
