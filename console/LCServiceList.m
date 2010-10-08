//
//  LCServiceList.m
//  Lithium Console
//
//  Created by James Wilson on 9/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCServiceList.h"
#import "LCCustomer.h"
#import "LCService.h"

@implementation LCServiceList

#pragma mark "Constructors"

+ (LCServiceList *) serviceListForCustomer:(id)initCustomer
{
	return [[[LCServiceList alloc] initWithCustomer:initCustomer] autorelease];
}

- (LCServiceList *) initWithCustomer:(id)initCustomer
{
	self = [self initWithSource:initCustomer
						xmlName:@"svcregistry_list"
					   forClass:[LCService class]
					   usingIndexKey:nil];
	if (!self) return nil;

	self.customer = initCustomer;
		
	return self;
}

- (void) dealloc
{
	[customer release];
	[super dealloc];
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	[super xmlParserDidFinish:rootNode];
	
	/* Refresh each process */
	for (LCEntity *entity in self.objects)
	{
		[entity highPriorityRefresh];
	}
}

#pragma mark "Properties"

@synthesize customer;
- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index
{
	[super insertObject:obj inObjectsAtIndex:index];
	LCEntity *entity = (LCEntity *)obj;
	[entity normalPriorityRefresh];
}

@end
