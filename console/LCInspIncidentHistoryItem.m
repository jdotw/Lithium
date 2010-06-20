//
//  LCInspIncidentHistoryItem.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspIncidentHistoryItem.h"

#import "LCInspIncidentHistoryViewController.h"

@implementation LCInspIncidentHistoryItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspIncidentHistoryItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"30-Day History (...)"];
	
	self.incidentList = [[LCIncidentList new] autorelease];
	incidentList.customer = [(LCEntity *)target customer];
	incidentList.delegate = self;
	incidentList.entity = target;
	incidentList.maxCount = [NSNumber numberWithInt:20];
	incidentList.startLowerSeconds = [NSNumber numberWithFloat:[[[NSDate date] addTimeInterval:(86400.0 * -30.0)] timeIntervalSince1970]];
	[incidentList highPriorityRefresh];	
	
	LCInspIncidentHistoryViewController *viewController = [[LCInspIncidentHistoryViewController alloc] initWithTarget:target incidentList:incidentList];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (void) dealloc
{
	[incidentList setDelegate:nil];
	[incidentList release];
	[super dealloc];
}

- (void) inclistRefreshFinished:(id)sender
{
	/* Set title */
	if ([[incidentList incidents] count] >= 20)
	{ [self setDisplayString:@"30-Day History (20+)"]; }
	else
	{ [self setDisplayString:[NSString stringWithFormat:@"30-Day History (%i)", [[incidentList incidents] count]]]; }
}

@synthesize incidentList;

@end
