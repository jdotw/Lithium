//
//  LCObjectTree.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTree.h"

#import "LCObject.h"
#import "LCObjectTreeHeaderItem.h"

@implementation LCObjectTree

#pragma mark "Constructors"

- (id) initWithContainer:(LCContainer *)initContainer;
{
	self = [self init];
	if (self)
	{
		self.container = initContainer;
		
		/* Create header item per object */
		for (LCObject *object in container.children)
		{
			LCObjectTreeHeaderItem *header = [[LCObjectTreeHeaderItem alloc] initWithObject:object showAllMetrics:NO];
			[self insertObject:header inItemsAtIndex:items.count];
			[header autorelease];
		}

		/* Listen for changes in the container */
		[container addObserver:self 
				 forKeyPath:@"children"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					context:NULL];	
	}
	return self;
}

- (id) initWithObject:(LCObject *)object;
{
	self = [self init];
	if (self)
	{
		/* Create header item for object */
		LCObjectTreeHeaderItem *header = [[LCObjectTreeHeaderItem alloc] initWithObject:object showAllMetrics:YES];
		[self insertObject:header inItemsAtIndex:items.count];
		[header autorelease];
	}
	return self;
}

- (id) init
{
	self = [super init];
	if (self)
	{
		items = [[NSMutableArray array] retain];
	}
	return self;
}

- (void) dealloc
{
	[container removeObserver:self forKeyPath:@"children"];
	[items release];
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
		for (LCObject *object in [change objectForKey:NSKeyValueChangeNewKey])
		{
			LCObjectTreeHeaderItem *header = [[LCObjectTreeHeaderItem alloc] initWithObject:object showAllMetrics:NO];
			[self insertObject:header inItemsAtIndex:items.count];
			[header autorelease];
			[outlineView expandAllItemsUsingPreferences];
		}
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCObject *object in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			NSMutableArray *removeArray = [NSMutableArray array];
			for (LCObjectTreeHeaderItem *item in self.items)
			{ if (item.object == object) [removeArray addObject:item]; }
			for (LCObjectTreeHeaderItem *item in removeArray)
			{ [self removeObjectFromItemsAtIndex:[self.items indexOfObject:item]];	}
		}
	}
}

#pragma mark "Items Accessor"

@synthesize items;

- (void) insertObject:(id)item inItemsAtIndex:(unsigned int)index
{
	[items insertObject:item atIndex:index];
}
- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[items removeObjectAtIndex:index];
}

#pragma mark "Properties"

@synthesize container;



@end
