//
//  LCContainerTreeDeviceItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeDeviceItem.h"


@implementation LCContainerTreeDeviceItem

#pragma mark "Constructors"

- (id) initWithDevice:(LCDevice *)initDevice
{
	self = [super init];
	
	if (self)
	{
		self.device = initDevice;
		
		childrenFilter = [[NSPredicate predicateWithFormat:@"%K != %@ AND %K != %@", @"name", @"service", @"name", @"procpro"] retain];
		children = (NSMutableArray *) [[device.children filteredArrayUsingPredicate:childrenFilter] mutableCopy];
		
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

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Device";
}

#pragma mark "Properties"

@end
