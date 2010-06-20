//
//  LCDarkGreyView.m
//  Lithium Console
//
//  Created by James Wilson on 8/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCDarkGreyView.h"


@implementation LCDarkGreyView

- (id)initWithFrame:(NSRect)frame 
{
    [super initWithFrame:frame];
    return self;
}

- (void)drawRect:(NSRect)rect 
{
	[[NSColor colorWithCalibratedRed:51/.0/256.0 green:51.0/256.0 blue:51.0/256.0 alpha:1.0] setFill];
	NSRectFill([self bounds]);
}

@end
