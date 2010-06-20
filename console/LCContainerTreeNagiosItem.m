//
//  LCContainerTreeNagiosItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeNagiosItem.h"


@implementation LCContainerTreeNagiosItem

- (id) initWithDevice:(LCDevice *)initDevice
{
	self = [super init];
	
	if (self)
	{
		self.device = initDevice;
		
		self.nagiosContainer = [device.childrenDictionary objectForKey:@"nagios"];
		children = (NSMutableArray *) [[nagiosContainer.children mutableCopy] retain];
		
		/* FIX needs to handle nagiosContainer not existing yet */
		
		/* Listen for changes in the child list */
		[nagiosContainer addObserver:self 
							forKeyPath:@"children"
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							   context:NULL];	
		
		self.isContainerTreeLeaf = NO;							   
	}
	
	return self;
}

- (void) dealloc
{
	[nagiosContainer removeObserver:self forKeyPath:@"children"];
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Nagios";
}

#pragma mark "Properties"

@synthesize nagiosContainer;


@end
