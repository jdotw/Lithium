//
//  LCContainerTreeServicesItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeServicesItem.h"


@implementation LCContainerTreeServicesItem

- (id) initWithDevice:(LCDevice *)initDevice
{
	self = [super init];
	
	if (self)
	{
		self.device = initDevice;
		
		self.servicesContainer = [device.childrenDictionary objectForKey:@"service"];
		children = (NSMutableArray *) [[servicesContainer.children mutableCopy] retain];
		
		/* FIX needs to handle servicesContainer not existing yet */
		
		/* Listen for changes in the child list */
		[servicesContainer addObserver:self 
				 forKeyPath:@"children"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					context:NULL];	
		
		self.isContainerTreeLeaf = NO;							   
	}
	
	return self;
}

- (void) dealloc
{
	[servicesContainer removeObserver:self forKeyPath:@"children"];
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Services";
}

#pragma mark "Properties"

@synthesize servicesContainer;

@end
