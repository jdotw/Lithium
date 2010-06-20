//
//  LCInspectorBackgroundView.m
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowser2InspectorView.h"


@implementation LCBrowser2InspectorView

- (void) drawRect:(NSRect)rect
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:rect];
	[[NSColor colorWithCalibratedWhite:39.0/256.0 alpha:1.0] setFill];
	[path fill];		
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
	
	
	[super drawRect:rect];
}

- (BOOL) isOpaque
{ return YES; }

@end
