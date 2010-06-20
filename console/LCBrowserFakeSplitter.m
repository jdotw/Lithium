//
//  LCBrowserFakeSplitter.m
//  Lithium Console
//
//  Created by James Wilson on 12/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserFakeSplitter.h"


@implementation LCBrowserFakeSplitter

- (void) drawRect:(NSRect)rect
{
	NSImage *handle = [NSImage imageNamed:@"greysplitviewhandle.png"];
	[handle drawInRect:[self bounds]
			  fromRect:NSMakeRect(0, 0, [handle size].width, [handle size].height)
			 operation:NSCompositeSourceOver
			  fraction:1.0];
}

@end
