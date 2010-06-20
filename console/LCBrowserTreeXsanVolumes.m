//
//  LCBrowserTreeXsanVolumes.m
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeXsanVolumes.h"

#import "LCCustomerList.h"


@implementation LCBrowserTreeXsanVolumes

#pragma mark "Constructors"

- (LCBrowserTreeXsanVolumes *) init
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
							forKeyPath:@"volumes" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							   context:NULL];	
		[children addObjectsFromArray:[customer.xsanList volumes]];
	}
	
	self.isBrowserTreeLeaf = NO;

	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[customer.xsanList removeObserver:self forKeyPath:@"volumes"];
	}
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the Xsan Volumes list */
	if ([keyPath isEqualToString:@"volumes"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCEntity *addedVolume in [change objectForKey:NSKeyValueChangeNewKey])
			{ [self insertObject:addedVolume inChildrenAtIndex:[children count]]; }
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCEntity *removedVolume in [change objectForKey:NSKeyValueChangeOldKey])
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:removedVolume]]; }
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
				 forKeyPath:@"volumes" 
				 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				 context:NULL];	
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *removedCustomer in [change objectForKey:NSKeyValueChangeOldKey])
			{ [removedCustomer.xsanList removeObserver:self forKeyPath:@"volumes"]; }
		}
		
	}
}

#pragma mark "Properties"

- (NSString *) displayString
{
	return @"Volumes";
}

- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{ [children insertObject:obj atIndex:index]; }
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{ [children removeObjectAtIndex:index]; }

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"hard_disk_16.tif"];
}

- (BOOL) selectable
{ return NO; }

@end
