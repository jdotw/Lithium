//
//  LCBrowserTreeFaultsRoot.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeFaultsRoot.h"

#import "LCBrowserTreeIncidents.h"
#import "LCBrowserTreeCases.h"

@implementation LCBrowserTreeFaultsRoot

#pragma mark "Constructors"

- (LCBrowserTreeFaultsRoot *) init
{
	[super init];
	
	self.incidents = [[LCBrowserTreeIncidents new] autorelease];
	self.cases = [[LCBrowserTreeCases new] autorelease];
	self.children = [NSMutableArray arrayWithObjects:incidents, cases, nil];
	
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[incidents release];
	[cases release];
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"FAULTS";
}

@synthesize incidents;
@synthesize cases;


@end
