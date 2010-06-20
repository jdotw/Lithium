//
//  LCBrowserTreeIncidents.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeIncidents.h"

@implementation LCBrowserTreeIncidents

#pragma mark "Constructors"

- (LCBrowserTreeIncidents *) init
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
	return @"Incidents";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"event_16.tif"];
}

- (BOOL) selectable
{ return YES; }

@end
