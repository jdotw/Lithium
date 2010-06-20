//
//  LCActivityController.m
//  Lithium Console
//
//  Created by James Wilson on 16/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActivityList.h"
#import "LCActivityController.h"
#import "LCXMLRequestQueue.h"

static LCActivityController *masterController = nil;

@implementation LCActivityController

#pragma mark "Initialisation"

+ (LCActivityController *) activityController
{
	if (masterController)
	{
		[[masterController window] makeKeyAndOrderFront:self];
		return nil; 
	}
	else
	{
		masterController = [[LCActivityController alloc] init];
		return masterController; 
	}
}

- (LCActivityController *) init
{	
	/* Sort descriptors */
	NSSortDescriptor *sortDesc;
	activitySortDescriptors = [[NSMutableArray array] retain];
	sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"priority" ascending:NO] autorelease];
	[activitySortDescriptors addObject:sortDesc];
	sortDesc = [[[NSSortDescriptor alloc] initWithKey:@"status" ascending:NO] autorelease];
	[activitySortDescriptors addObject:sortDesc];
	
	/* Super class init */
	[super initWithWindowNibName:@"ActivityViewer"];
	[self window];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[activitySortDescriptors release];
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove master */
	masterController = nil;
	
	/* Remove bindings/observers */
	[controllerAlias setContent:nil];
	
	/* Autorelease */
	[self autorelease];
}

#pragma mark "Accessor Methods"

- (LCActivityList *) activityList
{ return [LCActivityList masterList]; }

- (NSMutableArray *) activitySortDescriptors
{ return activitySortDescriptors; }

@synthesize controllerAlias;
@synthesize backView;
@synthesize activitySortDescriptors;
@end
