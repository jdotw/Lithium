//
//  LCContainerTreeCell.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeCell.h"

#import "LCRoundedBezierPath.h"

@implementation LCContainerTreeCell

#pragma mark "Drawing Method"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Set Clip */
	NSBezierPath *clipPath = [LCRoundedBezierPath pathInRect:[controlView bounds]];
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[clipPath addClip];
	
	if ([[representedObject class] isSubclassOfClass:[LCEntity class]])
	{
		/* Entity */
		[self drawInteriorWithFrame:cellFrame inView:controlView forEntity:representedObject];
	}
	else
	{ 
		/* Header Item */
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forRootItem:representedObject];
	}	
	
	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

#pragma mark "Root Item"

- (void) drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forRootItem:(LCContainerTreeItem *)item
{
	float xOffset = 0.0;
	
	/* Draw Background */
	NSRect backgroundFrame = NSMakeRect(0, NSMinY(cellFrame),
										NSWidth([[controlView enclosingScrollView] frame]), 
										NSHeight(cellFrame));	
	NSImage *headerGradient = [NSImage imageNamed:@"ObjectHeaderGradient.png"];
	[headerGradient drawInRect:backgroundFrame
					  fromRect:NSMakeRect(0.0, 0.0, 18.0, 18.0) 
					 operation:NSCompositeSourceOver 
					  fraction:0.25];
	
	/* Top Bars */
	NSBezierPath *line = [NSBezierPath bezierPath];
	[line moveToPoint:NSMakePoint(0, NSMinY(cellFrame)-0.5)];
	[line lineToPoint:NSMakePoint(NSWidth([[controlView enclosingScrollView] frame]), NSMinY(cellFrame)-0.5)];
	[line setLineWidth:1.0];
	[[NSColor colorWithCalibratedWhite:66.0/256.0 alpha:0.76] setStroke];
	[line stroke];
	line = [NSBezierPath bezierPath];
	[line moveToPoint:NSMakePoint(0, NSMinY(cellFrame)-1.5)];
	[line lineToPoint:NSMakePoint(NSWidth([[controlView enclosingScrollView] frame]), NSMinY(cellFrame)-1.5)];
	[line setLineWidth:1.0];
	[[NSColor colorWithCalibratedWhite:32.0/256.0 alpha:0.46] setStroke];
	[line stroke];	
	
	/* Remeber defaults */
	NSFont *defaultFont = [self font];
	
	/* Set bold/color */
	[self setFont:[[NSFontManager sharedFontManager] convertFont:[self font] toHaveTrait:NSBoldFontMask]];
	[self setTextColor:[NSColor colorWithCalibratedWhite:0.0 alpha:0.3]];
	
	/* Draw text */
	[super drawInteriorWithFrame:NSMakeRect(cellFrame.origin.x+xOffset,
											cellFrame.origin.y+1,
											cellFrame.size.width, cellFrame.size.height) inView:controlView];	
	
	/* Restore defaults */
	[self setFont:defaultFont];	
}

#pragma mark "Entity Drawing"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity
{
	float xOffset = 0.0;
	
	/* Background */
	NSRect backgroundFrame = NSMakeRect(0, NSMinY(cellFrame),
										NSWidth([[controlView enclosingScrollView] frame]), 
										NSHeight(cellFrame)+2);
	if (firstRow)
	{
		backgroundFrame = NSMakeRect(NSMinX(backgroundFrame), 
									 NSMinY(backgroundFrame) - 4.0f, 
									 NSWidth(backgroundFrame), 
									 NSHeight(backgroundFrame) + 4.0f);
	}
	NSBezierPath *backgroundPath = [NSBezierPath bezierPathWithRect:backgroundFrame];
	if ([[controlView window] isMainWindow])
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.18] setFill]; }
	else
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.08] setFill]; }
	[backgroundPath fill];
	
	if (lastRow)
	{
		NSImage *bottomGradient = [NSImage imageNamed:@"IndentTopGradient.png"];	/* It's flipped, hence the mismatch filename */
		[bottomGradient drawInRect:NSMakeRect(0.0, NSMaxY(cellFrame) - 2.0f, NSWidth([[controlView enclosingScrollView] frame]), 3.0f)
						  fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
						 operation:NSCompositeSourceOver 
						  fraction:0.05];
	}
	
	if (firstRow)
	{
		NSImage *topGradient = [NSImage imageNamed:@"IndentBottomGradient.png"];	/* It's flipped, hence the mismatch filename */
		[topGradient drawInRect:NSMakeRect(0.0, NSMinY(cellFrame)-4.0f, NSWidth([[controlView enclosingScrollView] frame]), 4.0f)
					   fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
					  operation:NSCompositeSourceOver 
					   fraction:0.18];	
	}
	
	/* Draw dot */
	NSImage *image;
	switch (entity.opState)
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
		[image drawInRect:NSMakeRect(cellFrame.origin.x + xOffset, cellFrame.origin.y+(0.18 * cellFrame.size.height)-1, cellFrame.size.height * 0.8, cellFrame.size.height * 0.8)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	}
	
	/* Create reduced-size rectangle */
	[self setTextColor:[NSColor blackColor]];
	NSRect textRect = NSMakeRect(cellFrame.origin.x + 12 + xOffset,
								 cellFrame.origin.y,
								 cellFrame.size.width,cellFrame.size.height);
	
	/* Draw and restore colour */
	[super drawInteriorWithFrame:textRect inView:controlView];	
	[self setTextColor:[NSColor blackColor]];
}

#pragma mark "Properties"

@synthesize representedObject;
@synthesize lastRow;
@synthesize firstRow;

@end
