//
//  LCInspPastIncidentsItem.m
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspPastIncidentsItem.h"

#import "LCInspPastIncidentsViewController.h"

@implementation LCInspPastIncidentsItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspPastIncidentsItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"30-Day Incident History (...)"];

	self.incidentList = [[LCIncidentList new] autorelease];
	incidentList.customer = [(LCEntity *)target customer];
	incidentList.delegate = self;
	incidentList.entity = target;
	incidentList.maxCount = [NSNumber numberWithInt:20];
	incidentList.startLowerSeconds = [NSNumber numberWithFloat:[[[NSDate date] addTimeInterval:(86400.0 * -30.0)] timeIntervalSince1970]];
	[incidentList highPriorityRefresh];	

	LCInspPastIncidentsViewController *viewController = [[LCInspPastIncidentsViewController alloc] initWithTarget:target incidentList:incidentList];
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
	{ [self setDisplayString:@"30-Day Incident History (20+)"]; }
	else
	{ [self setDisplayString:[NSString stringWithFormat:@"30-Day Incident History (%i)", [[incidentList incidents] count]]]; }
}

@synthesize incidentList;

@end
