//
//  LCShadowTextFieldCell.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "LCShadowTextFieldCell.h"


@implementation LCShadowTextFieldCell

- (void) awakeFromNib
{
	[self setTextColor:[NSColor whiteColor]];
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Establish size of text */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys: [self font], NSFontAttributeName, nil];
	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	
	/* Create centered rect */
	NSRect centeredRect = NSMakeRect(cellFrame.origin.x + 1,
									 (cellFrame.origin.y + (0.5 * (cellFrame.size.height - textSize.height))),
//									 cellFrame.origin.y,
									 cellFrame.size.width - 1,
									 cellFrame.size.height);
	
	/* Font adjust */
//	if ([[[self font] fontName] hasPrefix:@"Bank"])
//	{
//		if (cellFrame.origin.y > 1.0)
//		{ centeredRect.origin.y -= 2.0; }
//		else
//		{ 
//			centeredRect.origin.y = 0;
//		}
//	}

	/* Create shadow */
	theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.35]];

	/* Draw using super-class */
	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(context); 
	[theShadow set];
	[super drawInteriorWithFrame:centeredRect inView:controlView];
	CGContextRestoreGState(context);
	
	/* Cleanup */
	[theShadow release];
}

- (BOOL) altRow
{ return altRow; }

- (void) setAltRow:(BOOL)flag
{ altRow = flag; }

- (int) colIndex
{ return colIndex; }

- (void) setColIndex:(int)index
{ colIndex = index; }

@end
