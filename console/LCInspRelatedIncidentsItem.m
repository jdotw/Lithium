//
//  LCInspRelatedIncidentsItem.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspRelatedIncidentsItem.h"

#import "LCInspRelatedIncidentsViewController.h"

@implementation LCInspRelatedIncidentsItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspRelatedIncidentsItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Related Incidents"];
	
	self.incidentList = [[LCIncidentList new] autorelease];
	incidentList.customer = [(LCEntity *)target customer];
	incidentList.delegate = self;
	[incidentList highPriorityRefresh];	
	
	LCInspRelatedIncidentsViewController *viewController = [[LCInspRelatedIncidentsViewController alloc] initWithTarget:target incidentList:incidentList];
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
	[incidentList scoreRelevanceToEntity:target];
	
	NSSortDescriptor *sortDescriptor = [[NSSortDescriptor alloc] initWithKey:@"relevanceScore" ascending:NO];
	[[incidentList incidents] sortUsingDescriptors:[NSArray arrayWithObject:sortDescriptor]];
	[sortDescriptor autorelease];
}

@synthesize incidentList;

@end
