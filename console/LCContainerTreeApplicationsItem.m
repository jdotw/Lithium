//
//  LCContainerTreeApplicationsItem.m
//  Lithium Console
//
//  Created by James Wilson on 15/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCContainerTreeApplicationsItem.h"


@implementation LCContainerTreeApplicationsItem

#pragma mark "Constructors"

- (id) initWithDevice:(LCDevice *)initDevice
{
	self = [super init];
	
	if (self)
	{
		self.device = initDevice;
		
		NSPredicate *filter = [NSPredicate predicateWithFormat:@"%K == %@ OR %K == %@", @"name", @"service", @"name", @"procpro"];
		children = (NSMutableArray *) [[device.children filteredArrayUsingPredicate:filter] mutableCopy];
		
		/* Listen for changes in the child list */
		[device addObserver:self 
				 forKeyPath:@"children"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					context:NULL];	
		
		self.isContainerTreeLeaf = NO;
	}
	
	return self;
}

- (void) dealloc
{
	[device removeObserver:self forKeyPath:@"children"];
	[super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the container list */
	int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
	if (changeType == NSKeyValueChangeInsertion)
	{
		NSPredicate *filter = [NSPredicate predicateWithFormat:@"%K == %@ OR %K == %@", @"name", @"service", @"name", @"procpro"];
		for (LCEntity *entity in [[change objectForKey:NSKeyValueChangeNewKey] filteredArrayUsingPredicate:filter])
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


#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Applications";
}

@end
