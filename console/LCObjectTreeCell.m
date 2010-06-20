//
//  LCObjectTreeCell.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeCell.h"

#import "LCObjectTreeMetricViewController.h"
#import "LCRoundedBezierPath.h"

@implementation LCObjectTreeCell

#pragma mark "Drawing Method"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Set Clip */
	NSBezierPath *clipPath = [LCRoundedBezierPath pathInRect:[controlView bounds]];
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[clipPath addClip];

	/* Check for final row */
	if (lastRow)
	{
		NSBezierPath *line = [NSBezierPath bezierPath];
		[line moveToPoint:NSMakePoint(0, NSMaxY(cellFrame)+1.5)];
		[line lineToPoint:NSMakePoint(NSWidth([[controlView enclosingScrollView] frame]), NSMaxY(cellFrame)+1.5)];
		[line setLineWidth:1.0];
		[[NSColor colorWithCalibratedWhite:66.0/256.0 alpha:0.4] setStroke];
		[line stroke];
		line = [NSBezierPath bezierPath];
		[line moveToPoint:NSMakePoint(0, NSMaxY(cellFrame)+0.5)];
		[line lineToPoint:NSMakePoint(NSWidth([[controlView enclosingScrollView] frame]), NSMaxY(cellFrame)+0.5)];
		[line setLineWidth:1.0];
		[[NSColor colorWithCalibratedWhite:32.0/256.0 alpha:0.46] setStroke];
		[line stroke];
		
	}
	
	if ([representedObject class] == [LCObjectTreeHeaderItem class])
	{
		/* Header item */
		LCObjectTreeHeaderItem *headerItem = representedObject;
		[self drawInteriorWithFrame:cellFrame inView:controlView forEntity:headerItem.object];
	}
	else
	{ 
		/* Metric Item */
		LCObjectTreeMetricItem *metricItem = representedObject;
		[self drawItemInteriorWithFrame:cellFrame inView:controlView forMetrics:metricItem];
	}
	

	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

- (void) drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity
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
		[image drawInRect:NSMakeRect(cellFrame.origin.x + xOffset, cellFrame.origin.y+(0.18 * cellFrame.size.height)-2, cellFrame.size.height * 0.8, cellFrame.size.height * 0.8)
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	}
	
	/* Create reduced-size rectangle */
	[self setTextColor:[NSColor blackColor]];
	NSRect textRect = NSMakeRect(cellFrame.origin.x + 12 + xOffset,
								 cellFrame.origin.y + 0.5,
								 cellFrame.size.width,cellFrame.size.height);
	
	/* Special displaystring handling for procpro/service */
	if ([[entity.parent name] isEqualToString:@"procpro"] || [[entity.parent name] isEqualToString:@"service"])
	{
		[self setStringValue:[NSString stringWithFormat:@"%@ on %@", entity.displayString, [entity.device displayString]]];
	}
	
	
	/* Draw and restore colour */
	[super drawInteriorWithFrame:textRect inView:controlView];	
	[self setTextColor:[NSColor blackColor]];
}

- (void) drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forMetrics:(LCObjectTreeMetricItem *)metricItem
{
	/* Background */
	NSRect backgroundFrame = NSMakeRect(0, NSMinY(cellFrame),
										NSWidth([[controlView enclosingScrollView] frame]), 
										NSHeight(cellFrame));
	NSBezierPath *backgroundPath = [NSBezierPath bezierPathWithRect:backgroundFrame];
	if ([[controlView window] isMainWindow])
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.18] setFill]; }
	else
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.08] setFill]; }
	[backgroundPath fill];
	NSImage *bottomGradient = [NSImage imageNamed:@"IndentTopGradient.png"];	/* It's flipped, hence the mismatch filename */
	[bottomGradient drawInRect:NSMakeRect(0.0, NSMaxY(cellFrame) - 2.0f, NSWidth([[controlView enclosingScrollView] frame]), 3.0f)
				   fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
				  operation:NSCompositeSourceOver 
				   fraction:0.05];
	NSImage *topGradient = [NSImage imageNamed:@"IndentBottomGradient.png"];	/* It's flipped, hence the mismatch filename */
	[topGradient drawInRect:NSMakeRect(0.0, NSMinY(cellFrame), NSWidth([[controlView enclosingScrollView] frame]), 4.0f)
					  fromRect:NSMakeRect(0.0, 0.0, 6.0, 6.0)
					 operation:NSCompositeSourceOver 
					  fraction:0.18];
	
	/* Draw Content (metric grid) */
	float rowHeight = 16.0;
	CGFloat requiredHeight = metricItem.displayedMetricViewControllers.count * 16.0;
	NSRect contentFrame = NSMakeRect(NSMinX([[controlView enclosingScrollView] frame]) + 20.0f,
									 NSMinY(cellFrame) + 2.0,
									 NSWidth([[controlView enclosingScrollView] frame]) - 50.0f, 
									 requiredHeight);
	float colWidth = contentFrame.size.width * 0.5f;
	int col = 0;
	int row = 0;
	for (LCObjectTreeMetricViewController *viewController in metricItem.displayedMetricViewControllers)
	{
		NSRect metricFrame = NSMakeRect(NSMinX(contentFrame) + (col * colWidth),
										NSMinY(contentFrame) + (row * rowHeight),
										colWidth, rowHeight);
		
		[[viewController view] setFrame:metricFrame];
		[controlView addSubview:[viewController view]];
		
		/* Increment Col/Row */
		if (col == 0)
		{
			col++;
		}
		else if (col == 1)
		{
			col = 0;
			row++;
		}
	}
}

#pragma mark "Observed Object"

@synthesize representedObject;
@synthesize firstRow;
@synthesize lastRow;
@synthesize expanded;

@end
