//
//  LTIncident.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTIncident.h"

@implementation LTIncident

- (LTIncident *) init
{
	self = [super init];
	if (self)
	{
		actions = [[NSMutableArray array] retain];
		actionDict = [[NSMutableDictionary dictionary] retain];
	}
	return self;
}

- (void) dealloc
{
	[startDate release];
	[endDate release];
	[raisedValue release];
	[resourceAddress release];
	[entityDescriptor release];
	[metric release];
	[actions release];
	[actionDict release];
	[super dealloc];
}

@synthesize identifier;
@synthesize startDate;
@synthesize endDate;
@synthesize caseIdentifier;
@synthesize raisedValue;
@synthesize resourceAddress;
@synthesize entityDescriptor;
@synthesize metric;
@synthesize actions;
@synthesize actionDict;

@end
