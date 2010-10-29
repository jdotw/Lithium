//
//  LCShadowPopupCell.m
//  Lithium Console
//
//  Created by James Wilson on 29/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LCShadowPopupCell.h"


@implementation LCShadowPopupCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	if (![self isEnabled]) return;
	
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
	}
	
	[super drawInteriorWithFrame:centeredRect inView:controlView];
//	[self setTextColor:[NSColor blackColor]];
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

- (void)drawTitleWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	NSColor *color;
	if (drawingToolTip) color = [NSColor blackColor];
	else color = [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.8];
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
						  color, NSForegroundColorAttributeName,
						  [self font], NSFontAttributeName,
						  nil];
	[[self titleOfSelectedItem] drawInRect:cellFrame withAttributes:attr];
}


@end
