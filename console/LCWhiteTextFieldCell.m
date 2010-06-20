//
//  LCWhiteTextFieldCell.m
//  Lithium Console
//
//  Created by James Wilson on 9/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWhiteTextFieldCell.h"


@implementation LCWhiteTextFieldCell

- (void) awakeFromNib
{
	[self setTextColor:[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:1.0]];
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Establish size of text */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys: [self font], NSFontAttributeName, nil];
	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	
	/* Create centered rect */
	NSRect centeredRect = NSMakeRect(cellFrame.origin.x + 1,
									 (cellFrame.origin.y + (0.5 * (cellFrame.size.height - textSize.height))),
									 cellFrame.size.width - 1,
									 cellFrame.size.height);
	
	/* Font adjust */
	if ([[[self font] fontName] hasPrefix:@"Bank"])
	{
		if (cellFrame.origin.y > 1.0)
		{ centeredRect.origin.y -= 2.0; }
		else
		{ 
			centeredRect.origin.y = 0;
		}
	}
	
	/* Create shadow */
	NSShadow *theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(1.0, -1.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.2]];
	
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
