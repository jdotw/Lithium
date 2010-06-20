//
//  LCBrowserTreeProcesses.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeProcesses.h"
#import "LCCustomerList.h"
#import "LCCustomer.h"
#import "LCObject.h"

@interface LCBrowserTreeProcesses (private)

- (void) observeCustomer:(LCCustomer *)customer;
- (void) unobserveCustomer:(LCCustomer *)customer;
- (void) resetOpState;

@end

@implementation LCBrowserTreeProcesses

#pragma mark "Constructors"

- (LCBrowserTreeProcesses *) init
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	self.opState = -1;
	self.rowHeight = 15.0;

	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		for (LCEntity *entity in customer.processProfileList.objects)
		{ 
			[self insertObject:entity inChildrenAtIndex:children.count];
			[entity addObserver:self
					 forKeyPath:@"opState"
						options:NSKeyValueObservingOptionNew
						context:nil];
		}
		[self observeCustomer:customer];
	}
	[self resetOpState];
	
	/* Observe change in customers */
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"array"
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];		
	
	self.isBrowserTreeLeaf = NO;

	return self;
}

- (void) dealloc
{
	for (LCEntity *obj in children)
	{
		[obj removeObserver:self forKeyPath:@"opState"];
	}
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ [self unobserveCustomer:customer]; }
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void) resetOpState
{
	int highestOpState = -1;
	for (LCEntity *entity in children)
	{
		if (entity.opState > highestOpState) highestOpState = entity.opState;
	}
	self.opState = highestOpState;
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the customers documentr list */
	if ([keyPath isEqualToString:@"objects"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCObject *obj in [change objectForKey:NSKeyValueChangeNewKey])
			{ 
				[self insertObject:obj inChildrenAtIndex:children.count]; 
				[obj addObserver:self
					  forKeyPath:@"opState"
						 options:NSKeyValueObservingOptionNew
						 context:nil];
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCObject *obj in [change objectForKey:NSKeyValueChangeOldKey])
			{ 
				[obj removeObserver:self forKeyPath:@"opState"];
				[self removeObjectFromChildrenAtIndex:[children indexOfObject:obj]]; 
			}
		}
	}
	
	/* Change in the customer list */
	if ([keyPath isEqualToString:@"array"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeNewKey])
			{ [self observeCustomer:customer]; }
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeOldKey])
			{ [self unobserveCustomer:customer]; }
		}
	}
	
	/* Check in entity opState */
	if ([keyPath isEqualToString:@"opState"])
	{
		[self resetOpState];
	}	
}

- (void) observeCustomer:(LCCustomer *)customer
{
	[customer.processProfileList addObserver:self 
						   forKeyPath:@"objects" 
							  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)  
							  context:NULL];
}

- (void) unobserveCustomer:(LCCustomer *)customer
{
	[customer.processProfileList removeObserver:self forKeyPath:@"objects"];
}


#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Processes";
}

- (BOOL) selectable
{ return YES; }

@end
