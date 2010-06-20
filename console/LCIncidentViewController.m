//
//  LCIncidentViewController.m
//  Lithium Console
//
//  Created by James Wilson on 20/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncidentViewController.h"


@implementation LCIncidentViewController

+ (LCIncidentViewController *) controllerForIncident:(LCIncident *)initIncident
{
	return [[[LCIncidentViewController alloc] initForIncident:initIncident] autorelease];
}

- (LCIncidentViewController *) initForIncident:(LCIncident *)initIncident
{
	[self init];
	[self setIncident:initIncident];
	
	return self;
}

- (LCIncidentViewController *) init
{
	[super init];

	[NSBundle loadNibNamed:@"GenericIncidentView" owner:self];
	
	return self;
}

- (void) dealloc
{
	[view release];
	[objectController release];
	[incident release];
	[super dealloc];
}

- (void) removeViewAndContent
{
	[view removeFromSuperview];
	[objectController setContent:nil];
}

#pragma mark Accessor Methods

- (LCIncident *) incident
{ return incident; }
- (void) setIncident:newIncident
{
	if (incident) [incident release];
	incident = [newIncident retain]; 
}

- (NSView *) view
{ return view; }



@synthesize view;
@synthesize objectController;
@end
