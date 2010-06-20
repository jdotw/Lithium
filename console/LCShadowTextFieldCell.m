//
//  LCShadowTextFieldCell.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCShadowTextFieldCell.h"


@implementation LCShadowTextFieldCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Establish size of text */
	NSSize textSize = [[self stringValue] sizeWithAttributes:[NSDictionary dictionaryWithObjectsAndKeys: [self font], NSFontAttributeName, nil]];
	
	/* Create centered rect */
	NSRect centeredRect;
	if (NSHeight(cellFrame) < 14)
	{
		centeredRect = NSMakeRect(cellFrame.origin.x + 1,
										 cellFrame.origin.y,
										 cellFrame.size.width - 1,
										 cellFrame.size.height);
	}
	else
	{
		centeredRect = NSMakeRect(cellFrame.origin.x + 1,
										 (cellFrame.origin.y + (0.5 * (cellFrame.size.height - textSize.height))),
										 cellFrame.size.width - 1,
										 cellFrame.size.height);
	}
	
	/* Create shadow */
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.35]];

	/* Draw using super-class */
	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(context); 
	if (!drawingToolTip)
	{
		[theShadow set];
		[self setTextColor:[NSColor colorWithCalibratedWhite:0.92 alpha:1.0]];
	}
	else
	{ [self setTextColor:[NSColor blackColor]]; }

	[super drawInteriorWithFrame:centeredRect inView:controlView];
	[self setTextColor:[NSColor blackColor]];
	CGContextRestoreGState(context);
	
	/* Cleanup */
	[theShadow release];
}

- (void)drawWithExpansionFrame:(NSRect)cellFrame inView:(NSView *)view
{
	drawingToolTip = YES;
	[super drawWithExpansionFrame:cellFrame inView:view];
	drawingToolTip = NO;
}

@end
