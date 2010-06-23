//
//  LCShadowTextField.m
//  Lithium Console
//
//  Created by James Wilson on 15/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCShadowTextField.h"


@implementation LCShadowTextField

- (void) awakeFromNib
{
	[self setTextColor:[NSColor whiteColor]];
}

- (void) drawRect:(NSRect)rect
{
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:3.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.35]];
	[theShadow set];
	[super drawRect:rect];
	[theShadow release];
}


@end
