//
//  LCMetricGraphWindowBackground.m
//  Lithium Console
//
//  Created by James Wilson on 14/12/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCMetricGraphWindowBackground.h"


@implementation LCMetricGraphWindowBackground

- (void) drawRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:rect];
	[[NSColor colorWithCalibratedWhite:0.0 alpha:0.7] setFill];
	[path fill];
}

@end
