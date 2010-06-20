//
//  LCActionList.m
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionList.h"

@implementation LCActionList

#pragma mark "Constructors"

+ (LCActionList *) actionListForCustomer:(LCCustomer *)initCustomer
{
	return [[[LCActionList alloc] initWithCustomer:initCustomer] autorelease];
}


- (LCActionList *) initWithCustomer:(LCCustomer *)initCustomer
{
	self = [super initWithSource:initCustomer
						 xmlName:@"action_list"
						forClass:[LCAction class]
				   usingIndexKey:@"taskID"];
	if (!self) return nil;
	
	self.customer = initCustomer;
	
	return self;
}

- (void) dealloc 
{
	[customer release];
	[super dealloc];
}

- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index
{
	LCAction *action = (LCAction *)obj;
	action.hostEntity = customer;
	[super insertObject:obj inObjectsAtIndex:index];
}

#pragma mark "Accessor Methods"
@synthesize customer;

@end
