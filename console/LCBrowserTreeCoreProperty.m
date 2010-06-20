//
//  LCBrowserTreeCoreProperty.m
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserTreeCoreProperty.h"

@implementation LCBrowserTreeCoreProperty

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer
{
	[super init];
	
	self.customer = initCustomer;
	self.displayString = customer.desc;
	self.isBrowserTreeLeaf = YES;
	
	return self;
}

- (void) dealloc
{
	[customer release];
	[super dealloc];
}

@synthesize customer;
@synthesize contentControllerClass;

@end
