//
//  LCBrowserTreeNetworkScan.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeNetworkScan.h"


@implementation LCBrowserTreeNetworkScan

#pragma mark "Constructors"

- (id) init
{
	[super init];
	
	self.isBrowserTreeLeaf = YES;
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Network Scan";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"web_grey_16.tif"];
}

- (BOOL) selectable
{ return YES; }

@end
