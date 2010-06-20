//
//  LCInspActiveIncidentsViewController.m
//  Lithium Console
//
//  Created by James Wilson on 15/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspActiveIncidentsViewController.h"
#import "LCIncidentController.h"

#import "LCEntity.h"

@implementation LCInspActiveIncidentsViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspActiveIncidentsViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];
	
	[tableView setRoundedSelection:YES];
	
	return self;
}

- (float) defaultHeight
{ 
	float minHeight = [[(LCEntity *)target incidents] count] * 13.0;
	if (minHeight < 120.0)
	{
		[[tableView enclosingScrollView] setAutohidesScrollers:YES];
		return minHeight + 2.0; 
	}
	return 120.0; 
}

- (IBAction) tableViewDoubleClicked:(NSArray *)selectedObjects
{
	for (LCIncident *incident in selectedObjects)
	{
		[[LCIncidentController alloc] initForIncident:incident];
	}
}

- (NSString *) nibName
{ return @"InspectorActiveIncidentsView"; }

@end
