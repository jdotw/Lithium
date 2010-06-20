//
//  LCInspServiceConfigViewController.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspServiceConfigViewController.h"


@implementation LCInspServiceConfigViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspServiceConfigViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];

	/* Create service */
	[self setService:[LCService serviceWithObject:initTarget]];
	[[self service] performXmlRefresh];
	[[self service] setDelegate:self];
	[[service scriptList] highPriorityRefresh];
	[[service configVariables] highPriorityRefresh];
	
	return self;
}

- (void) dealloc
{
	[service release];
	[super dealloc];
}

- (float) rowHeight
{ return 120.0; }

@synthesize service;

- (NSString *) nibName
{ return @"InspectorServiceConfigView"; }

@end
