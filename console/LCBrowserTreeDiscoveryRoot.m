//
//  LCBrowserTreeDiscoveryRoot.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeDiscoveryRoot.h"

#import "LCBrowserTreeBonjour.h"
#import "LCBrowserTreeNetworkScan.h"

@implementation LCBrowserTreeDiscoveryRoot

#pragma mark "Constructors"

- (id) init
{
	[super init];
	
	LCBrowserTreeBonjour *bonjour = [LCBrowserTreeBonjour new];
	LCBrowserTreeNetworkScan *scan = [LCBrowserTreeNetworkScan new];	
	self.children = [NSMutableArray arrayWithObjects:bonjour, scan, nil];
	[bonjour autorelease];
	[scan autorelease];
	
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"DISCOVERY";
}



@end
