//
//  LCWhiteTextField.m
//  Lithium Console
//
//  Created by James Wilson on 9/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWhiteTextField.h"


@implementation LCWhiteTextField

- (void) awakeFromNib
{
	[self setTextColor:[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:1.0]];
}

- (void) drawRect:(NSRect)rect
{
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(1.0, -1.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.2]];
	[theShadow set];
	[super drawRect:rect];
	[theShadow release];
}

@end
