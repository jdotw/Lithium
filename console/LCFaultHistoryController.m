//
//  LCFaultHistoryController.m
//  Lithium Console
//
//  Created by James Wilson on 30/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCFaultHistoryController.h"
#import "LCIncident.h"
#import "LCIncidentController.h"
#import "LCCase.h"
#import "LCCaseController.h"

@implementation LCFaultHistoryController

#pragma mark Initialisation

- (LCFaultHistoryController *) initForEntity:(LCEntity *)initEntity
{
	[super init];

	/* Set entity */
	[self setEntity:initEntity];

	/* Incident list */
	incidentList = [LCIncidentList new];
	incidentList.customer = entity.customer;
	[incidentList setEntity:entity];
	[incidentList highPriorityRefresh];
	
	/* Case List */
	caseList = [LCCaseList new];
	caseList.customer = entity.customer;
	[caseList setEntity:entity];
	[caseList highPriorityRefresh];

	/* Super class init */
	[super initWithWindowNibName:@"FaultHistory"];

	/* Window setup */
	[self buildToolbar];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	[[self window] makeKeyAndOrderFront:self];

	/* Set sort descriptors */
	[self setIncidentArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"startDateShortString" ascending:NO] autorelease]]];
	[self setCaseArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"openDateShortString" ascending:NO] autorelease]]];
		
	return self;
}

- (void) dealloc
{
	[controllerAlias setContent:nil];
	[incidentList release];
	[caseList release];
	[entity release];
	[super dealloc];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"FaultHistoryToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setVisible:NO];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Refresh incidents */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Incidents"] autorelease];
	[tbitem setLabel:@"Refresh Incidents"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshIncidentsClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshIncident.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Incidents"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Refresh cases */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Cases"] autorelease];
	[tbitem setLabel:@"Refresh Cases"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshCasesClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshCase.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Cases"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	[[self window] setToolbar:toolbar];
}

- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag 
{ return [toolbarItems objectForKey: identifier]; }

- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarDefaultItems; }

- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarDefaultItems; }

- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarSelectableItems; }

- (IBAction) toggleToolbarClicked:(id)sender
{ [[self window] toggleToolbarShown:sender]; }

#pragma mark "User Interface Action Methods"

- (IBAction) incidentTableViewDoubleClicked:(NSArray *)selectedObjects
{
	LCIncident *inc;
	for (inc in selectedObjects)
	{
		[[LCIncidentController alloc] initForIncident:inc];
	}
}

- (IBAction) caseTableViewDoubleClicked:(NSArray *)selectedObjects
{
	LCCase *cas;
	for (cas in selectedObjects)
	{
		[[LCCaseController alloc] initWithCase:cas];
	}
}

- (IBAction) refreshIncidentsClicked:(id)sender
{
	[incidentList refreshWithPriority:XMLREQ_PRIO_HIGH];
}

- (IBAction) refreshCasesClicked:(id)sender
{
	[caseList refreshWithPriority:XMLREQ_PRIO_HIGH];	
}

#pragma mark Accessor Methods

- (LCEntity *) entity
{ return entity; }

- (void) setEntity:(LCEntity *)otherEntity
{ 
	[entity release];
	entity = [otherEntity retain]; 
}

- (LCCaseList *) caseList
{ return caseList; }

- (LCIncidentList *) incidentList
{ return incidentList; }

- (NSArray *) incidentArraySortDescriptors 
{ return incidentArraySortDescriptors; }
- (void) setIncidentArraySortDescriptors:(NSArray *)array
{
	if (incidentArraySortDescriptors) [incidentArraySortDescriptors release];
	incidentArraySortDescriptors = [array retain];
}

- (NSArray *) caseArraySortDescriptors 
{ return caseArraySortDescriptors; }
- (void) setCaseArraySortDescriptors:(NSArray *)array
{
	if (caseArraySortDescriptors) [caseArraySortDescriptors release];
	caseArraySortDescriptors = [array retain];
}

@synthesize controllerAlias;
@synthesize backView;
@synthesize caseList;
@synthesize incidentList;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@end
