//
//  LCShadowEntityTextField.m
//  Lithium Console
//
//  Created by James Wilson on 16/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCShadowEntityCell.h"


@implementation LCShadowEntityCell

- (void) awakeFromNib
{
	[self setTextColor:[NSColor whiteColor]];
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(1.0, -.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.5]];
	NSDictionary *attr;
	if (!drawingToolTip)
	{
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSColor whiteColor], NSForegroundColorAttributeName,
							  [self font], NSFontAttributeName,
							  theShadow, NSShadowAttributeName,
							  nil];
	}
	else
	{
		attr = [NSDictionary dictionaryWithObjectsAndKeys:
							  [NSColor blackColor], NSForegroundColorAttributeName,
							  [self font], NSFontAttributeName,
							  nil];
	}
	
	
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
		[image drawInRect:NSMakeRect(cellFrame.origin.x, cellFrame.origin.y+(0.18 * cellFrame.size.height)-1, cellFrame.size.height * 0.7, cellFrame.size.height * 0.7)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	}

//	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	NSRect textRect = NSMakeRect(cellFrame.origin.x + 12, cellFrame.origin.y, cellFrame.size.width - 12, cellFrame.size.height - 2);
	//	[[self stringValue] drawAtPoint:NSMakePoint(cellFrame.origin.x-1, cellFrame.origin.y) withAttributes:attr];
	[[self stringValue] drawInRect:textRect withAttributes:attr];
			
	[theShadow release];
	
}

- (void)drawWithExpansionFrame:(NSRect)cellFrame inView:(NSView *)view
{
	drawingToolTip = YES;
	[super drawWithExpansionFrame:cellFrame inView:view];
	drawingToolTip = NO;
}

@end
