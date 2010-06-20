//
//  LCServiceScript.m
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCServiceScript.h"


@implementation LCServiceScript

#pragma mark "Constructors"

- (id) init
{
	self = [super initWithType:@"service"];
	if (!self) return nil;
	
	[self.xmlTranslation setObject:@"port" forKey:@"port"];
	[self.xmlTranslation setObject:@"protocol" forKey:@"protocol"];
	[self.xmlTranslation setObject:@"transport" forKey:@"transport"];	
	
	return self;
}

- (void) dealloc
{
	[protocol release];
	[transport release];
	[super dealloc];
}

#pragma mark "Properties"
@synthesize port;
@synthesize protocol;
@synthesize transport;

@end
