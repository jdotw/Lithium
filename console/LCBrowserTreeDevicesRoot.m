//
//  LCBrowserTreeDevicesRoot.m
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeDevicesRoot.h"

#import "LCCustomerList.h"

@implementation LCBrowserTreeDevicesRoot

#pragma mark "Constructors"

- (LCBrowserTreeDevicesRoot *) init
{
	[super init];

	self.children = (NSMutableArray *) [[LCCustomerList masterList] array];
	
	/* Listen for changes in the customer list */
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"array" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];	
	
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the container list */
	int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
	if (changeType == NSKeyValueChangeInsertion)
	{
		for (LCEntity *entity in [change objectForKey:NSKeyValueChangeNewKey])
		{ 
			[self insertObject:entity inChildrenAtIndex:[children count]]; 
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:@"BrowserTreeItemAdded" object:self];
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCEntity *entity in [change objectForKey:NSKeyValueChangeOldKey])
		{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:entity]]; }
	}
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"MONITORED DEVICES";
}

@end
