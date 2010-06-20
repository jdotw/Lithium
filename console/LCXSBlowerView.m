//
//  LCXSBlowerView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSBlowerView.h"


@implementation LCXSBlowerView

#pragma mark "Constructors"

- (LCXSBlowerView *) initWithBlower:(LCEntity *)initBlower inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	blowerObject = [initBlower retain];
	
	blowerImage = [[NSImage imageNamed:@"xsblower.png"] retain];
	
    return self;
}

- (void) dealloc
{
	[blowerObject release];
	[super dealloc];
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	[super removeFromSuperview];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	/* Draw blower image */
	[blowerImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				  fromRect:NSMakeRect(0,0,[blowerImage size].width,[blowerImage size].height)
				 operation:NSCompositeSourceOver
				  fraction:1.0];
	
	/* Status Dot */
	NSRect statusRect = NSMakeRect((NSWidth([self bounds]) - 6) / 2, (NSHeight([self bounds]) - 6) / 2, 6, 6);
	NSBezierPath *statusPath = [NSBezierPath bezierPathWithOvalInRect:statusRect];
	[[blowerObject opStateColor] setFill];
	[statusPath fill];
	[[NSColor grayColor] setStroke];
	[statusPath stroke];
}


@synthesize blowerObject;
@synthesize blowerImage;
@end
