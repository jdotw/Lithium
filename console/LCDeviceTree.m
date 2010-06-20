//
//  LCDeviceTree.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTree.h"

#import "LCDeviceTreeDeviceItem.h"

@implementation LCDeviceTree

#pragma mark "Constructors"

- (id) initWithSite:(LCSite *)initSite
{
	self = [super init];
	if (!self) return nil;

	self.site = initSite;
	items = [[NSMutableArray array] retain];
	
	/* Create initial items */
	for (LCDevice *device in site.children)
	{
		LCDeviceTreeDeviceItem *devItem = [[LCDeviceTreeDeviceItem alloc] initWithDevice:device];
		[self insertObject:devItem inItemsAtIndex:[items count]];
		[devItem autorelease];
	}
	
	return self;
}

- (id) initWithCustomer:(LCCustomer *)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	items = [[NSMutableArray array] retain];
	
	/* Create initial items */
	for (LCSite *childSite in customer.children)
	{
		for (LCDevice *device in childSite.children)
		{
			LCDeviceTreeDeviceItem *devItem = [[LCDeviceTreeDeviceItem alloc] initWithDevice:device];
			[self insertObject:devItem inItemsAtIndex:[items count]];
			[devItem autorelease];
		}
	}
	
	return self;
}


- (void) dealloc
{
	[site release];
	[customer release];
	[items release];
	[super dealloc];
}

#pragma mark "Items Accessor"

@synthesize items;

- (void) insertObject:(id)item inItemsAtIndex:(unsigned int)index
{
	[items insertObject:item atIndex:index];
}
- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[items removeObjectAtIndex:index];
}

#pragma mark "Properties"

@synthesize site;
@synthesize customer;

@end
