//
//  MBTopView.m
//  ModuleBuilder
//
//  Created by James Wilson on 16/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBTopView.h"


@implementation MBTopView

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rects
{
	NSImage *image = [NSImage imageNamed:@"topback.png"];
	if (image)
	{
		[image drawInRect:[self bounds]
				 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
	}
}

@end
