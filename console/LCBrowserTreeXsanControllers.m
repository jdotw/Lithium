//
//  LCBrowserTreeXsanControllers.m
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeXsanControllers.h"
#import "LCCustomerList.h"

@implementation LCBrowserTreeXsanControllers

#pragma mark "Constructors"

- (LCBrowserTreeXsanControllers *) init
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	
	/* Listen for changes in the customer list */
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"array" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];	

	/* Listen for changes in Xsan Volumes in all existing customers */
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[customer.xsanList addObserver:self 
							forKeyPath:@"controllers" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							   context:NULL];
		[children addObjectsFromArray:[customer.xsanList controllers]];
	}

	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[customer.xsanList removeObserver:self forKeyPath:@"controllers"];
	}
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in a customers Xsan Controller list */
	if ([keyPath isEqualToString:@"controllers"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCEntity *addedController in [change objectForKey:NSKeyValueChangeNewKey])
			{ [self insertObject:addedController inChildrenAtIndex:[children count]]; }
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCEntity *removedController in [change objectForKey:NSKeyValueChangeOldKey])
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:removedController]]; }
		}		
	}
	
	/* Change in the customer list */
	else if ([keyPath isEqualToString:@"array"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCCustomer *addedCustomer in [change objectForKey:NSKeyValueChangeNewKey])
			{ 
				[addedCustomer.xsanList addObserver:self
				 forKeyPath:@"controllers" 
				 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				 context:NULL];	
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *removedCustomer in [change objectForKey:NSKeyValueChangeOldKey])
			{ [removedCustomer.xsanList removeObserver:self forKeyPath:@"controllers"]; }
		}		
	}
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Controllers";
}

- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{ [children insertObject:obj atIndex:index]; }
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{ [children removeObjectAtIndex:index]; }

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"bluecomputer_16.tif"];
}

- (BOOL) selectable
{ return NO; }

@end
