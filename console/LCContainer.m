//
//  LCContainer.m
//  Lithium Console
//
//  Created by James Wilson on 27/09/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCContainer.h"

#import "LCObject.h"

@implementation LCContainer

- (LCContainer *) init
{
	[super init];

	[self.xmlTranslation setObject:@"itemListState" forKey:@"item_list_state_int"];
	[self.xmlTranslation setObject:@"isModuleBuilder" forKey:@"modb"];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (Class) childClass
{ return [LCObject class]; }

@synthesize itemListState;

@synthesize isModuleBuilder;

@end
