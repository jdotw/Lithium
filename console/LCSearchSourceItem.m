//
//  LCSearchSourceItem.m
//  Lithium Console
//
//  Created by James Wilson on 25/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCSearchSourceItem.h"

#import "LCCustomerList.h"

@implementation LCSearchSourceItem

- (id) initWithCustomer:(LCCustomer *)initCustomer
{
	self = [self init];
	if (self != nil)
	{
		self.customer = initCustomer;
		if (customer)
		{
			/* Source is a specific customer */
			LCSearchRequest *req = [[LCSearchRequest new] autorelease];
			req.entity = customer;
			[req addObserver:self
				  forKeyPath:@"results"
					 options:NSKeyValueObservingOptionOld|NSKeyValueObservingOptionNew
					 context:nil];
			self.searchRequests = [NSArray arrayWithObject:req];
			self.displayString = customer.desc;
		}
		else 
		{
			NSMutableArray *reqs = [NSMutableArray array];
			for (LCCustomer *newCustomer in [LCCustomerList masterArray])
			{
				LCSearchRequest *req = [[LCSearchRequest new] autorelease];
				req.entity = newCustomer;
				[req addObserver:self
					  forKeyPath:@"results"
						 options:NSKeyValueObservingOptionOld|NSKeyValueObservingOptionNew
						 context:nil];
				[reqs addObject:req];
			}
			self.searchRequests = reqs;
			self.displayString = @"All";
		}

	}
	return self;
}

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		results = [[NSMutableArray array] retain];
	}
	return self;
}

- (void) dealloc
{
	for (LCSearchRequest *req in searchRequests)
	{
		[req removeObserver:self forKeyPath:@"results"];
	}
	[customer release];
	[searchRequests release];
	[displayString release];
	[results release];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the search results */
	int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
	if (changeType == NSKeyValueChangeInsertion)
	{
		for (LCEntity *entity in [change objectForKey:NSKeyValueChangeNewKey])
		{ 
			[self insertObject:entity inResultsAtIndex:results.count];
		}
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCEntity *entity in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			if ([results containsObject:entity])
			{ [self removeObjectFromResultsAtIndex:[results indexOfObject:entity]]; }
		}
	}
}

#pragma mark "Search Methods"

- (void) search
{
	for (LCSearchRequest *req in searchRequests)
	{
		[req search];
	}
}

#pragma mark "Properties"

@synthesize customer;
@synthesize searchRequests;
@synthesize displayString;

@synthesize searchString;
- (void) setSearchString:(NSString *)value
{
	[searchString release];
	searchString = [value copy];
	
	for (LCSearchRequest *req in searchRequests)
	{
		req.searchString = searchString;
	}
}

@synthesize results;
- (void) insertObject:(LCEntity *)entity inResultsAtIndex:(unsigned int)index
{
	[results insertObject:entity atIndex:index];
}
- (void) removeObjectFromResultsAtIndex:(unsigned int)index
{
	[results removeObjectAtIndex:index];
}

@end
