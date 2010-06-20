//
//  LCProcessProfileList.m
//  Lithium Console
//
//  Created by James Wilson on 16/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCProcessProfileList.h"
#import "LCCustomer.h"


@implementation LCProcessProfileList

#pragma mark "Constructors"

+ (LCProcessProfileList *) profileListForCustomer:(id)initCustomer
{
	return [[[LCProcessProfileList alloc] initForCustomer:initCustomer] autorelease];
}

- (LCProcessProfileList *) initForCustomer:(id)initCustomer
{
	self = [self initWithSource:initCustomer
						xmlName:@"procregistry_list"
					   forClass:[LCEntity class]
				  usingIndexKey:nil];
	if (!self) return nil;
	
	return self;
}

- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index
{
	[super insertObject:obj inObjectsAtIndex:index];
	LCEntity *entity = (LCEntity *)obj;
	[entity normalPriorityRefresh];
}

@end
