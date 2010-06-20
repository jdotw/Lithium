//
//  LCDeviceTreeIncidentItem.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTreeIncidentItem.h"

@implementation LCDeviceTreeIncidentItem

#pragma mark "Constructors"

- (id) initWithIncident:(LCIncident *)initIncident
{
	self = [super init];
	if (!self) return nil;
	
	self.incident = initIncident;		
	self.isDeviceTreeLeaf = YES;
	
	return self;
}

- (void) dealloc
{
	[incident release];
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return [NSString stringWithFormat:@"%@ %@ %@ %@", 
	[incident.entity.container desc],
	[incident.entity.object desc],
	[incident.entity.metric desc],
	[incident.entity.trigger desc]];
}

#pragma mark "Properties"

@synthesize incident;


@end
