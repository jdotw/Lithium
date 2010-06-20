//
//  LCDeviceTreeCell.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTreeCell.h"
#import "LCDeviceTreeDeviceItem.h"
#import "LCDeviceTreeIncidentItem.h"

#import "LCRoundedBezierPath.h"

@interface LCDeviceTreeCell (private)
- (void) drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forDevice:(LCDevice *)device;
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forIncident:(LCIncident *)incident;
@end

@implementation LCDeviceTreeCell

#pragma mark "Drawing Method"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Set Clip */
	NSBezierPath *clipPath = [LCRoundedBezierPath pathInRect:[controlView bounds]];
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[clipPath addClip];
	
	if ([[representedObject class] isSubclassOfClass:[LCDeviceTreeDeviceItem class]])
	{
		/* Device */
		LCDeviceTreeDeviceItem *devItem = (LCDeviceTreeDeviceItem *) representedObject;
		[self drawInteriorWithFrame:cellFrame inView:controlView forDevice:devItem.device];
	}
	else if ([[representedObject class] isSubclassOfClass:[LCDeviceTreeIncidentItem class]])
	{
		/* Incident */
		LCDeviceTreeIncidentItem *incItem = (LCDeviceTreeIncidentItem *) representedObject;
		[self drawInteriorWithFrame:cellFrame inView:controlView forIncident:incItem.incident];
	}
	
	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

#pragma mark "Device Item"

- (void) drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forDevice:(LCDevice *)device
{	
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
	float xOffset = 0.0;
	NSImage *image;
	switch (device.opState)
	{
		case -3:
			image = [NSImage imageNamed:@"NoDot.tiff"];
			break;
		case -2:
			image = [NSImage imageNamed:@"BlueDotFlipped.tiff"];
			break;
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
	if (![device opstateInteger])
	{ image = nil; }
	[image drawInRect:NSMakeRect(cellFrame.origin.x + xOffset, cellFrame.origin.y+(0.18 * cellFrame.size.height)-1, 
								 13.0, 13.0)
			 fromRect:NSMakeRect(0, 0, image.size.width, image.size.height)
			operation:NSCompositeSourceOver
			 fraction:1.0];
	
	/* Create reduced-size rectangle */
	[self setTextColor:[NSColor blackColor]];
	NSRect textRect = NSMakeRect(cellFrame.origin.x + 12 + xOffset,
								 cellFrame.origin.y,
								 cellFrame.size.width,cellFrame.size.height);
	
	/* Draw and restore colour */
	[super drawInteriorWithFrame:textRect inView:controlView];	
	[self setTextColor:[NSColor blackColor]];
}

#pragma mark "Incident Drawing"

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forIncident:(LCIncident *)incident
{
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
	float xOffset = 0.0;
	NSImage *image;
	switch (incident.entity.opState)
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
	if (![incident.entity opstateInteger])
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
