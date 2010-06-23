//
//  LCBackgroundView.m
//  Lithium Console
//
//  Created by James Wilson on 21/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBackgroundView.h"


@implementation LCBackgroundView

- (void) drawResizeIndicators:(NSRect)rect
{
}

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rects
{
	NSImage *image = [NSImage imageNamed:@"flatgrey.png"];
	if (image && [[self window] isMainWindow])
	{
		[image drawInRect:[self bounds]
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
	}
	else
	{
		[[NSColor colorWithCalibratedWhite:0.4 alpha:1.0] setFill];
		NSRectFill([self bounds]);
	}
}

@end
