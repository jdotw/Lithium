//
//  LCShadowTextFieldCell.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCShadowTextFieldCell.h"


@implementation LCShadowTextFieldCell

- (void) awakeFromNib
{
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Establish size of text */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys: [self font], NSFontAttributeName, nil];
	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	
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
	theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(1.0, -1.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.01]];

	/* Draw using super-class */
	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(context); 
	[theShadow set];
	if ([[controlView window] isMainWindow])
	{ [self setTextColor:[NSColor colorWithCalibratedWhite:0.98 alpha:1.0]]; }
	else
	{ [self setTextColor:[NSColor colorWithCalibratedWhite:0.98 alpha:0.6]]; }
	[super drawInteriorWithFrame:centeredRect inView:controlView];
	[self setTextColor:[NSColor blackColor]];
	CGContextRestoreGState(context);
	
	/* Cleanup */
	[theShadow release];
}

@end
