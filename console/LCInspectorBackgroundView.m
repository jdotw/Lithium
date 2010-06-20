//
//  LCInspectorBackgroundView.m
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorBackgroundView.h"


@implementation LCInspectorBackgroundView

- (void) drawRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:[self bounds]];
//	[[NSColor colorWithCalibratedWhite:33.0/256.0 alpha:0.9] setFill];
	[[NSColor colorWithCalibratedWhite:25.0/256.0 alpha:0.95] setFill];
	[path fill];
}

- (BOOL) isOpaque
{ return NO; }

@end
