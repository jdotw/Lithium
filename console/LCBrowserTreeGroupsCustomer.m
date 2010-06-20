//
//  LCBrowserTreeGroupsCustomer.m
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeGroupsCustomer.h"

#import "LCGroup.h"

@interface LCBrowserTreeGroupsCustomer (private)
- (void) updateOpstate;
@end

@implementation LCBrowserTreeGroupsCustomer

#pragma mark "Constructors"

- (LCBrowserTreeGroupsCustomer *) initWithCustomer:(LCCustomer *)initCustomer
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	
	self.opState = -1;
	self.customer = initCustomer;
	self.rowHeight = 15.0;
	[customer.groupTree highPriorityRefresh];
	[customer.groupTree addObserver:self 
						 forKeyPath:@"groups" 
							options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							context:NULL];
	[[self.customer groupTree] highPriorityRefresh];

	/* Build initial list */
	for (LCGroup *group in customer.groupTree.groups)
	{
		[self insertObject:group inChildrenAtIndex:children.count];
	}
	[self updateOpstate];
	self.isBrowserTreeLeaf = children.count ? NO : YES;
	
	return self;
}

- (void) dealloc
{
	for (LCGroup *group in children)
	{
		[group removeObserver:self forKeyPath:@"opState"];
	}
	[customer.groupTree removeObserver:self forKeyPath:@"groups"];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the root groups list */
	if ([keyPath isEqualToString:@"groups"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			/* Root group added */
			for (LCGroup *group in [change objectForKey:NSKeyValueChangeNewKey])
			{ [self insertObject:group inChildrenAtIndex:[customer.groupTree.groups indexOfObject:group]]; }
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			/* Root group removed */
			for (LCGroup *group in [change objectForKey:NSKeyValueChangeOldKey])
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:group]]; }
		}
		[self updateOpstate];
		self.isBrowserTreeLeaf = children.count ? NO : YES;
	}
	else if ([keyPath isEqualToString:@"opState"])
	{ [self updateOpstate]; }
}

#pragma mark "Op State"

- (void) updateOpstate
{
	int highestOpstate = -3;
	for (LCGroup *group in children)
	{
		if (group.opState > highestOpstate)
		{ highestOpstate = group.opState; }
	}
	if (self.opState != highestOpstate) self.opState = highestOpstate;
}

#pragma mark "Tree Item Methods"

- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{
	[super insertObject:obj inChildrenAtIndex:index];
	[obj addObserver:self 
		  forKeyPath:@"opState"
			 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
			 context:NULL];	
	
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[[children objectAtIndex:index] removeObserver:self forKeyPath:@"opState"];
	[super removeObjectFromChildrenAtIndex:index];
	
}

- (NSString *) displayString
{
	return customer.displayString;
}

- (NSImage *) treeIcon
{
	return nil;
}

- (BOOL) selectable
{ return YES; }

@synthesize customer;

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@-%i", [self className], self.customer.name]; }

@end
