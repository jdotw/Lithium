//
//  LCContainerTreeItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeItem.h"


@implementation LCContainerTreeItem

- (void) dealloc
{
	[childrenFilter release];
	[children release];
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
		for (LCEntity *entity in [[change objectForKey:NSKeyValueChangeNewKey] filteredArrayUsingPredicate:childrenFilter])
		{ [self insertObject:entity inChildrenAtIndex:[children count]]; }
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCEntity *entity in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			if ([children containsObject:entity])
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:entity]]; }
		}
	}
}

#pragma mark Accessors

@synthesize device;

- (NSString *) displayString
{ return @"--NONE--"; }

@synthesize rowHeight;

@synthesize children;
- (void) setChildren:(NSMutableArray *)value
{
	[children release];
	children = [value mutableCopy];
}
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index
{
	[children insertObject:obj atIndex:index]; 
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[children removeObjectAtIndex:index];
}

@synthesize isContainerTreeLeaf;

- (int) opState
{ return 0; }

- (BOOL) selectable
{ return YES; }

@end
