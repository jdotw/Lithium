//
//  LCBrowserTreeApplicationsRoot.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeApplicationsRoot.h"

#import "LCBrowserTreeServices.h"
#import "LCBrowserTreeProcesses.h"

@implementation LCBrowserTreeApplicationsRoot

#pragma mark "Constructors"

- (LCBrowserTreeApplicationsRoot *) init
{
	[super init];
	
	LCBrowserTreeServices *services = [[LCBrowserTreeServices new] autorelease];
	LCBrowserTreeProcesses *processes = [[LCBrowserTreeProcesses new] autorelease];
	
	self.children = [NSMutableArray arrayWithObjects:services, processes, nil];
	
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
	return @"APPLICATIONS";
}

@end
