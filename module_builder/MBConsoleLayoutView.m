//
//  MBConsoleLayoutView.m
//  ModuleBuilder
//
//  Created by James Wilson on 17/08/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBConsoleLayoutView.h"


@implementation MBConsoleLayoutView

- (void) drawRect:(NSRect)rect
{
	NSImage *image = [NSImage imageNamed:@"ConsoleViewBack.png"];
	[image drawInRect:[self bounds]
			 fromRect:NSMakeRect(0, 0, [image size].width, [image size].height)
			operation:NSCompositeSourceOver
			 fraction:1.0];
}

@end
