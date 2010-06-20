//
//  LCInsetNoSidesView.m
//  Lithium Console
//
//  Created by James Wilson on 21/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCInsetNoSidesView.h"


@implementation LCInsetNoSidesView

- (void) drawRect:(NSRect)rects
{
	NSBezierPath *path = [NSBezierPath bezierPathWithRect:self.bounds];
	[[NSColor colorWithCalibratedWhite:64.0/256.0 alpha:1.0] setFill];
	[path fill];
	
	CGFloat shadowWidth = 14.0;
	CGFloat shadowAlpha = 1.0;
	
	NSImage *image = [NSImage imageNamed:@"centre.png"];
	[image drawInRect:self.bounds
			 fromRect:NSMakeRect(0.0, 0.0, image.size.width, image.size.height)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	
	image = [NSImage imageNamed:@"top.png"];
	[image drawInRect:NSMakeRect(NSMinX(self.bounds), NSMaxY(self.bounds)-shadowWidth, NSWidth(self.bounds), shadowWidth) 
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0)
			operation:NSCompositeSourceOver 
			 fraction:shadowAlpha];
	
	if (![[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedWhite:0.7 alpha:0.2] setFill];
		[[NSBezierPath bezierPathWithRect:[self bounds]] fill];
	}	
}

@end
