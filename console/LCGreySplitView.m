//
//  LCGreySplitView.m
//  Lithium Console
//
//  Created by James Wilson on 12/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCGreySplitView.h"


@implementation LCGreySplitView

- (void)drawDividerInRect:(NSRect)aRect
{
	NSImage *image = [NSImage imageNamed:@"greysplitviewhandle.png"];
	[image setFlipped:YES];
	[image drawInRect:aRect
			 fromRect:NSMakeRect(0,0,[image size].width,[image size].height)
			operation:NSCompositeSourceOver 
			 fraction:1.0];
	[super drawDividerInRect:aRect];
}

@end
