//
//  LCBrowserMidToolbarView.m
//  Lithium Console
//
//  Created by James Wilson on 14/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserMidToolbarView.h"


@implementation LCBrowserMidToolbarView

- (void) drawRect:(NSRect)aRect
{
	NSImage *backImage = [NSImage imageNamed:@"thickdivider.png"];
	[backImage drawInRect:[self bounds]
				 fromRect:NSMakeRect(0,0,[backImage size].width,[backImage size].height)
				operation:NSCompositeSourceOver
				 fraction:1.0];
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
	
}

@end
