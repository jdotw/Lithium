//
//  LCInspIncidentHistoryViewController.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspIncidentHistoryViewController.h"
#import "LCIncidentController.h"

@implementation LCInspIncidentHistoryViewController

+ (LCInspIncidentHistoryViewController *) itemWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList
{
	return [[[LCInspIncidentHistoryViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList
{
	[super initWithTarget:initTarget];
	
	self.incidentList = initIncidentList;
	
	[tableView setRoundedSelection:YES];
	
	return self;
}

- (float) defaultHeight
{ 
	return 80.0;
}

- (void) dealloc
{
	[incidentList release];
	[super dealloc];
}

- (IBAction) tableViewDoubleClicked:(NSArray *)selectedObjects
{
	for (LCIncident *incident in selectedObjects)
	{
		[[LCIncidentController alloc] initForIncident:incident];
	}
}

@synthesize incidentList;

- (NSString *) nibName
{ return @"InspectorIncidentHistoryView"; }

@end
