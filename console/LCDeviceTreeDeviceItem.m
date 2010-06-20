//
//  LCDeviceTreeDeviceItem.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTreeDeviceItem.h"
#import "LCDeviceTreeIncidentItem.h"
#import "LCIncident.h"

@implementation LCDeviceTreeDeviceItem

#pragma mark "Constructors"

- (id) initWithDevice:(LCDevice *)initDevice
{
	self = [super init];
	
	if (self)
	{
		self.device = initDevice;

		children = [[NSMutableArray array] retain];
		for (LCIncident *incident in device.incidents)
		{
			LCDeviceTreeIncidentItem *incItem = [[LCDeviceTreeIncidentItem alloc] initWithIncident:incident];
			[self insertObject:incItem inChildrenAtIndex:children.count];
			[incItem autorelease];
		}
		if (children.count > 0)
		{ self.isDeviceTreeLeaf = NO; }
		else
		{ self.isDeviceTreeLeaf = YES; }
		
		/* Listen for changes in the child list */
		[device addObserver:self 
				 forKeyPath:@"incidents"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					context:NULL];	
		
	}
	
	return self;
}

- (void) dealloc
{
	[device release];
	[device removeObserver:self forKeyPath:@"incidents"];
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
		for (LCIncident *incident in [change objectForKey:NSKeyValueChangeNewKey])
		{ 
			LCDeviceTreeIncidentItem *incItem = [[LCDeviceTreeIncidentItem alloc] initWithIncident:incident];
			[self insertObject:incItem inChildrenAtIndex:children.count];
		}
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCIncident *incident in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			NSMutableArray *removeArray = [NSMutableArray array];
			for (LCDeviceTreeIncidentItem *incItem in children)
			{
				if (incItem.incident == incident)
				{ [removeArray addObject:incItem]; } 
			}
			for (LCDeviceTreeIncidentItem *incItem in removeArray)
			{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:incItem]]; } 
		}
	}
	
	if (children.count > 0)
	{ self.isDeviceTreeLeaf = NO; }
	else
	{ self.isDeviceTreeLeaf = YES; }
}


#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return device.displayString;
}

- (NSString *) uniqueIdentifier
{ 
	return device.uniqueIdentifier;
}

#pragma mark "Properties"

@synthesize device;

@end
