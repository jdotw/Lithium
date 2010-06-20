//
//  LCInspPastIncidentsViewController.m
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspPastIncidentsViewController.h"
#import "LCIncidentController.h"

@implementation LCInspPastIncidentsViewController

+ (LCInspPastIncidentsViewController *) itemWithTarget:(id)initTarget incidentList:(LCIncidentList *)initIncidentList
{
	return [[[LCInspPastIncidentsViewController alloc] initWithTarget:initTarget] autorelease];
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
{ return @"InspectorPastIncidentsView"; }

@end
