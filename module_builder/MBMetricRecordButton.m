//
//  MBMetricRecordButton.m
//  ModuleBuilder
//
//  Created by James Wilson on 7/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBMetricRecordButton.h"

@implementation MBMetricRecordButton

- (void) drawRect:(NSRect)rect
{
	MBMetric *metric = (MBMetric *) [viewController representedObject];
	
	/* Draw Background */
	NSBezierPath *backPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
	if ([[metric recordMethod] intValue] == 0)
	{ [[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill]; }
	else
	{ [[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:0.4] setFill]; }	
	[backPath fill];
	
	/* Draw Clicked over-lay */
	if (mouseIsDown)
	{
		NSBezierPath *mouseDownPath = [NSBezierPath bezierPathWithOvalInRect:self.bounds];
		[[NSColor colorWithCalibratedWhite:0.0 alpha:0.1] setFill];
		[mouseDownPath fill];
	}		
	
	/* Record Button */
	NSBezierPath *recordPath = [NSBezierPath bezierPathWithOvalInRect:NSMakeRect(4.0, 4.0, self.bounds.size.width - 8.0, self.bounds.size.height - 8.0)];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.6] setFill];
	[recordPath fill];
}

@end
