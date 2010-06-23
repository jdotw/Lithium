//
//  MBMetricSummaryButton.m
//  ModuleBuilder
//
//  Created by James Wilson on 23/04/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBMetricSummaryButton.h"


@implementation MBMetricSummaryButton

- (void) drawRect:(NSRect)rect
{
	MBMetric *metric = (MBMetric *) [viewController representedObject];
	
	/* Draw Background */
	NSBezierPath *backPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
	if ([metric showInSummary])
	{ [[NSColor colorWithCalibratedRed:107.0/256.0 green:168.0/256.0 blue:246.0/256.0 alpha:0.6] setFill]; }	
	else
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
	[backPath fill];
	
	/* Draw Clicked over-lay */
	if (mouseIsDown)
	{
		NSBezierPath *mouseDownPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[mouseDownPath fill];
	}		
	
	/* Summary Lines */
	NSBezierPath *linePath;
	linePath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds])+4.0, 
														   NSMidY([self bounds])-0.5, 
														   NSWidth([self bounds])-8.0,
														   1.0)];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.8] setFill];
	[linePath fill];
	linePath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds])+4.0, 
														   NSMidY([self bounds])-3.5, 
														   NSWidth([self bounds])-8.0,
														   1.0)];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.8] setFill];
	[linePath fill];
	linePath = [NSBezierPath bezierPathWithRect:NSMakeRect(NSMinX([self bounds])+4.0, 
														   NSMidY([self bounds])+2.5, 
														   NSWidth([self bounds])-8.0,
														   1.0)];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.8] setFill];
	[linePath fill];
}


@end
