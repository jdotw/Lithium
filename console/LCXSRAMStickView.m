//
//  LCXSRAMStickView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSRAMStickView.h"


@implementation LCXSRAMStickView

#pragma mark "Constructors"

- (LCXSRAMStickView *) initWithRAMStick:(LCEntity *)initStick inFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
	
	stickObject = [initStick retain];
	
	stickImage = [[NSImage imageNamed:@"xsramstick.png"] retain];
	
    return self;
}

- (void) dealloc
{
	[stickObject release];
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
	/* Draw drive image */
	[stickImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				fromRect:NSMakeRect(0,0,[stickImage size].width,[stickImage size].height)
			   operation:NSCompositeSourceOver
				fraction:1.0];
}


@synthesize stickObject;
@synthesize stickImage;
@end
