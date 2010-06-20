//
//  LCBrowserFlatBack.m
//  Lithium Console
//
//  Created by James Wilson on 25/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserFlatBack.h"


@implementation LCBrowserFlatBack

- (void) drawRect:(NSRect)rect
{
	NSBezierPath *backPath = [NSBezierPath bezierPathWithRect:[self bounds]];
	[[NSColor colorWithCalibratedWhite:50.0/256.0 alpha:1.0] setFill];
	[backPath fill];
	[super drawRect:rect];
}

@end
