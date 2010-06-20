//
//  LCGreyButtonView.m
//  Lithium Console
//
//  Created by James Wilson on 14/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCGreyButtonView.h"


@implementation LCGreyButtonView

- (void) drawRect:(NSRect)rect
{
	NSImage *image = [NSImage imageNamed:@"greybuttonrunner.tif"];
	[image drawInRect:[self bounds]
			 fromRect:NSMakeRect(0,0,[image size].width,[image size].height)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
}

@end
