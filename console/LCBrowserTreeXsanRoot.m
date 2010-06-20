//
//  LCBrowserTreeXsanRoot.m
//  Lithium Console
//
//  Created by James Wilson on 30/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeXsanRoot.h"

#import "LCBrowserTreeXsanVolumes.h"
#import "LCBrowserTreeXsanControllers.h"
#import "LCBrowserTreeXsanClients.h"
#import "LCCustomerList.h"

@implementation LCBrowserTreeXsanRoot

#pragma mark "Constructors"

- (LCBrowserTreeXsanRoot *) init
{
	[super init];

	/* Create items */
	items = [[NSMutableArray array] retain];
	
	/* Create Volumes item */
	[self insertObject:[LCBrowserTreeXsanVolumes new] inItemsAtIndex:0];

	/* Create Controllers item */
	[self insertObject:[LCBrowserTreeXsanControllers new] inItemsAtIndex:1];

	/* Create Clients item */
	[self insertObject:[LCBrowserTreeXsanClients new] inItemsAtIndex:2];
	
	/* Refresh xsanlists */
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[[customer xsanList] highPriorityRefresh];
	}
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"XSAN";
}

- (NSMutableArray *) children
{
	return items;
}

#pragma mark "Accessors"

@synthesize items;

- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index
{ [items insertObject:obj atIndex:index]; }

- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{ [items removeObjectAtIndex:index]; }

@end
