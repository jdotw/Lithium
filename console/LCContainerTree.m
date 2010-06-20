//
//  LCContainerTree.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTree.h"

#import "LCContainerTreeDeviceItem.h"
#import "LCContainerTreeNagiosItem.h"
#import "LCContainerTreeServicesItem.h"
#import "LCContainerTreeApplicationsItem.h"

@implementation LCContainerTree

#pragma mark "Constructors"

- (id) initWithDevice:(LCDevice *)initDevice;
{
	self = [super init];
	if (self)
	{
		self.device = initDevice;
		items = [[NSMutableArray array] retain];
		
		/* Device Item */
		LCContainerTreeDeviceItem *deviceItem = [[LCContainerTreeDeviceItem alloc] initWithDevice:self.device];
		[self insertObject:deviceItem inItemsAtIndex:[items count]];
		[deviceItem autorelease];

		/* Applications Item */
		LCContainerTreeApplicationsItem *appItem = [[LCContainerTreeApplicationsItem alloc] initWithDevice:self.device];
		[self insertObject:appItem inItemsAtIndex:[items count]];
		[appItem autorelease];		
		
		/* Nagios Item */
		if (self.device.useNagios)
		{
			LCContainerTreeNagiosItem *nagiosItem = [[LCContainerTreeNagiosItem alloc] initWithDevice:self.device];
			[self insertObject:nagiosItem inItemsAtIndex:[items count]];	
			[nagiosItem autorelease];
		}
	}
	return self;
}

- (void) dealloc
{
	[items release];
	[super dealloc];
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

@synthesize device;


@end
