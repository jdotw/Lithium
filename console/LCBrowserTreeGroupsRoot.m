//
//  LCBrowserTreeGroupsRoot.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeGroupsRoot.h"

#import "LCCustomer.h"
#import "LCCustomerList.h"
#import "LCBrowserTreeGroupsCustomer.h"

@implementation LCBrowserTreeGroupsRoot

#pragma mark "Constructors"

- (LCBrowserTreeGroupsRoot *) init
{
	[super init];

	children = [[NSMutableArray array] retain];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		LCBrowserTreeGroupsCustomer *groupCustomer = (LCBrowserTreeGroupsCustomer *) [[LCBrowserTreeGroupsCustomer alloc] initWithCustomer:customer];
		[self insertObject:groupCustomer inChildrenAtIndex:children.count];
	}
	
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
		for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeNewKey])
		{ 
			LCBrowserTreeGroupsCustomer *groupCustomer = (LCBrowserTreeGroupsCustomer *) [[LCBrowserTreeGroupsCustomer alloc] initWithCustomer:customer];
			[self insertObject:groupCustomer inChildrenAtIndex:children.count];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:@"BrowserTreeItemAdded" object:self];
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			NSMutableArray *removeArray = [NSMutableArray array];
			for (LCBrowserTreeGroupsCustomer *groupCustomer in children)
			{
				if (groupCustomer.customer == customer) 
				{ [removeArray addObject:groupCustomer]; }
			}
			for (LCBrowserTreeGroupsCustomer *groupCustomer in removeArray)
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:groupCustomer]]; }
		}
	}
}


#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"GROUPS";
}

@end
