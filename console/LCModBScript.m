//
//  LCModBScript.m
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCModBScript.h"


@implementation LCModBScript

#pragma mark "Constructors"

- (id) init
{
	self = [super initWithType:@"modb"];
	if (!self) return nil;
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

@end
