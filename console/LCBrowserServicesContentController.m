//
//  LCBrowserServicesContentController.m
//  Lithium Console
//
//  Created by James Wilson on 17/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserServicesContentController.h"


@implementation LCBrowserServicesContentController

- (id) initWithBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithBrowser:initBrowser];
	if (!self) return nil;

	/* Setup */
	self.stadiumTitle = @"Service Monitoring";
	
	/* Create object tree */
	self.objectTree = [[[LCObjectTree alloc] init] autorelease];
	
	/* Create initial object list */
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		for (LCObject *obj in customer.serviceList.objects)
		{
			[self insertObject:obj inObjectsAtIndex:objects.count];
			
			/* Create objectTree header item */
			LCObjectTreeHeaderItem *header = [[LCObjectTreeHeaderItem alloc] initWithObject:obj showAllMetrics:YES];
			[objectTree insertObject:header inItemsAtIndex:objectTree.items.count];			
			[header autorelease];
		}
		
		[customer.serviceList addObserver:self
									  forKeyPath:@"services"
										 options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
										 context:nil];
	}
	
	/* Listen for further changes in the customer list */
	[[LCCustomerList masterList] addObserver:self
								  forKeyPath:@"array"
									 options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
									 context:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		[customer.serviceList removeObserver:self forKeyPath:@"services"];
	}
	[super removedFromBrowserWindow];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	[super observeValueForKeyPath:keyPath
						 ofObject:object
						   change:change
						  context:context];
	
	int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
	if ([keyPath isEqualToString:@"services"])
	{
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCObject *obj in [change objectForKey:NSKeyValueChangeNewKey])
			{ 
				[self insertObject:obj inObjectsAtIndex:objects.count]; 
				LCObjectTreeHeaderItem *header = [[LCObjectTreeHeaderItem alloc] initWithObject:obj showAllMetrics:YES];
				[objectTree insertObject:header inItemsAtIndex:objectTree.items.count];
				[header autorelease];
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCObject *obj in [change objectForKey:NSKeyValueChangeOldKey])
			{ 
				NSMutableArray *removeArray = [NSMutableArray array];
				for (LCObjectTreeHeaderItem *item in objectTree.items)
				{
					if (item.object == obj) [removeArray addObject:item];
				}
				for (LCObjectTreeHeaderItem *item in removeArray)
				{ [objectTree removeObjectFromItemsAtIndex:[objectTree.items indexOfObject:item]]; }
				[self removeObjectFromObjectsAtIndex:[objects indexOfObject:obj]];
			}
		}		
	}
	else if ([keyPath isEqualToString:@"array"])
	{
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeNewKey])
			{ 
				[customer.serviceList addObserver:self
									   forKeyPath:@"services"
										  options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
										  context:nil];
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeOldKey])
			{ 
				[customer.serviceList removeObserver:self
										  forKeyPath:@"services"];
			}
		}
	}
	
}


@end
