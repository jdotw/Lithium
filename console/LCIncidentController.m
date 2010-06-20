//
//  IncidentViewerController.m
//  Lithium Console
//
//  Created by James Wilson on 18/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncidentController.h"
#import "LCCaseController.h"
#import "LCBrowser2Controller.h"
#import "LCMetricGraphDocument.h"
#import "LCMetricHistoryWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCActionListWindowController.h"
#import "LCAction.h"
#import "LCMetric.h"

@implementation LCIncidentController

#pragma mark "Initialisation"

- (LCIncidentController *) initForIncident:(LCIncident *)initIncident
{
	self.incident = initIncident;
	
	/* Drawer */
	self.drawerVisible = YES;

	/* Incident list */
	incidentList = [LCIncidentList new];
	incidentList.customer = incident.entity.customer;
	incidentList.entity = incident.entity;
	[incidentList highPriorityRefresh];

	/* Related Incident list */
	relatedIncidentList = [LCIncidentList new];
	relatedIncidentList.customer = incident.entity.customer;
	[relatedIncidentList highPriorityRefresh];
	[relatedIncidentList addObserver:self 
						  forKeyPath:@"refreshInProgress" 
							 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							 context:NULL];
	
	/* Case List */
	caseList = [LCCaseList new];
	caseList.customer = incident.entity.customer;
	caseList.entity = incident.entity;
	[caseList highPriorityRefresh];

	/* Object Tree */
	objectTree = (LCObjectTree *) [[LCObjectTree alloc] initWithObject:[[incident entity] object]];

	/* Set sort descriptors */
	[self setIncidentArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"startDateShortString" ascending:NO] autorelease]]];
	[self setRelatedIncidentArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"relevanceScore" ascending:NO] autorelease]]];
	[self setCaseArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"openDateShortString" ascending:NO] autorelease]]];	
	
	/* Super-class init */
	[super initWithWindowNibName:@"IncidentWindow"];
	[self window];
	[backgroundView setImage:[NSImage imageNamed:@"slateback.png"]];
	[topContentView setBackImage:[NSImage imageNamed:@"browser_objback.png"]];
	[objectTableView setRoundedSelection:YES];
	[objectOutlineView expandAllItemsUsingPreferences];		
	[self buildToolbar];
	
	/* Set container tree selection to 
	 * the entity in the incident
	 */
	if ([[incident entity] container])
	{
		NSIndexPath *indexPath = [NSIndexPath indexPathWithIndex:[[[[incident entity] device] children] indexOfObject:[[incident entity] container]]];
		if ([[incident entity] object]) 
		{ indexPath = [indexPath indexPathByAddingIndex:[[[[incident entity] container] children] indexOfObject:[[incident entity] object]]]; }
		if ([[incident entity] metric]) 
		{ indexPath = [indexPath indexPathByAddingIndex:[[[[incident entity] object] children] indexOfObject:[[incident entity] metric]]]; }
		if ([[incident entity] trigger]) 
		{ indexPath = [indexPath indexPathByAddingIndex:[[[[incident entity] metric] children] indexOfObject:[[incident entity] trigger]]]; }
		
		[containerTreeController setSelectionIndexPath:indexPath];
	}
	
	/* Refresh entity */
	[(LCEntity *)[[incident entity] object] highPriorityRefresh];
	
	/* Add action tableview refrsh timer */
	actionRefreshTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(refreshActionTableView) userInfo:nil repeats:YES] retain];

	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[incidentList release];
	[relatedIncidentList release];
	[caseList release];
	[actionRefreshTimer invalidate];
	[actionRefreshTimer release];
	[incidentArraySortDescriptors release];
	[relatedIncidentArraySortDescriptors release];
	[caseArraySortDescriptors release];
	[incident release];
	[super dealloc];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;

	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"BrowserToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];

	/* Refresh incident */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Incident"] autorelease];
	[tbitem setLabel:@"Refresh Incident"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshIncidentClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshIncident.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Incident"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Refresh entity */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Entity"] autorelease];
	[tbitem setLabel:@"Refresh Entity"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshEntityClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshEntity.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Entity"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* New case for incident */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Open Case for Incident"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (openCaseClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseOpenForSingle.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* New case for multiple incidents */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Open Case for Multiple"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (openCaseForMultipleClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseOpenForMultiple.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			
	
	/* Browse To */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Show in Browser"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (browseToSelectedClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"view_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Graph */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Metric"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (graphSelectedClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Chart.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Metric History */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Metric History"] autorelease];
	[tbitem setLabel:@"Metric History"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (metricHistoryClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"MetricHistory.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Metric History"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			
	
	/* Analyse */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Analyse Selected"] autorelease];
	[tbitem setLabel:@"Analyse Selected"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (analyseSelectedClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"TrendAnalysis.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Analyse Selected"];
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

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	/* Obtain selection */
	SEL action = [item action];
	
	if (action == @selector(openCaseClicked:) || 
		action == @selector(openCaseForMultipleClicked:) ||
		action == @selector(relatedSelectionOpenClicked:))
	{ if (![[[incident entity] customer] userIsNormal]) return NO; }
	
	if (action == @selector(executeActionClicked:))
	{ if (![[[incident entity] customer] userIsNormal]) return NO; }

	if (action == @selector(editActionsClicked:))
	{ if (![[[incident entity] customer] userIsAdmin]) return NO; }
	
	return YES;
}

#pragma mark "Refresh Methods"

- (IBAction) refreshIncidentClicked:(id)sender
{
	[[[[incident entity] customer] activeIncidentsList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

- (IBAction) refreshEntityClicked:(id)sender
{
	[[[incident entity] object] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove bindings/observers */
//	[containerTreeController setContent:nil];
	[relatedIncidentList removeObserver:self forKeyPath:@"refreshInProgress"];
//	[controllerAlias setContent:nil];
	
	/* Autorelease */
	[self autorelease];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"refreshInProgress"])
	{
		/* Calculate incident relevance */
		for (LCIncident *relatedIncident in [relatedIncidentList incidents])
		{
			/* Set local-Incident settings */
			if (relatedIncident.incidentID == incident.incidentID)
			{
				relatedIncident.relatedSelected = YES;
				relatedIncident.relatedSelectionDisabled = YES;
			}			
		}
		[relatedIncidentList scoreRelevanceToEntity:[incident entity]];
	}
}

#pragma mark "Case Opening Methods"

- (IBAction) openCaseClicked:(id)sender
{
	/* Create entity list */
	NSMutableArray *entityArray = [NSMutableArray array];
	[entityArray addObject:[incident entity]];
	
	/* Open Case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:entityArray];	
}

- (IBAction) openCaseForMultipleClicked:(id)sender
{
	/* Open new case sheet */
	[NSApp beginSheet:relatedSelectionWindow
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) relatedSelectionOpenClicked:(id)sender
{
	/* Create entity list */
	NSMutableArray *entityArray = [NSMutableArray array];
	for (LCIncident *relatedIncident in [relatedIncidentList incidents])
	{
		if (relatedIncident.relatedSelected == 1)
		{ [entityArray addObject:[relatedIncident entity]]; }
	}
	
	/* End Sheet */
	[NSApp endSheet:relatedSelectionWindow];	
	[relatedSelectionWindow close];

	/* Open Case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:entityArray];	
}

- (IBAction) relatedSelectionCancelClicked:(id)sender
{
	/* End Sheet */
	[NSApp endSheet:relatedSelectionWindow];
	[relatedSelectionWindow close];
}

- (IBAction) relatedSelectionSelectDeviceClicked:(id)sender
{
	for (LCIncident *relatedIncident in [relatedIncidentList incidents])
	{
		/* Skip it if selection is disabled */
		if (relatedIncident.relatedSelectionDisabled) continue;
		
		LCEntity *site = [[incident entity] site];
		LCEntity *relatedSite = [[relatedIncident entity] site];
		LCEntity *device = [[incident entity] device];
		LCEntity *relatedDevice = [[relatedIncident entity] device];
		
		if ([[site name] isEqualToString:[relatedSite name]] && [[device name] isEqualToString:[relatedDevice name]])
		{
			if (relatedIncident.relatedSelected)
			{ relatedIncident.relatedSelected = NO; }
			else
			{ relatedIncident.relatedSelected = YES; }
		}
	}
}

- (IBAction) relatedSelectionSelectSiteClicked:(id)sender
{
	for (LCIncident *relatedIncident in [relatedIncidentList incidents])
	{
		/* Skip it if selection is disabled */
		if (relatedIncident.relatedSelectionDisabled) continue;

		LCEntity *site = [[incident entity] site];
		LCEntity *relatedSite = [[relatedIncident entity] site];
		
		if ([[site name] isEqualToString:[relatedSite name]])
		{
			if (relatedIncident.relatedSelected)
			{ relatedIncident.relatedSelected = NO; }
			else
			{ relatedIncident.relatedSelected = YES; }
		}
	}	
}

- (IBAction) relatedSelectionSelectAllClicked:(id)sender
{
	for (LCIncident *relatedIncident in [relatedIncidentList incidents])
	{
		/* Skip it if selection is disabled */
		if (relatedIncident.relatedSelected)
		{ relatedIncident.relatedSelected = NO; }
		else
		{ relatedIncident.relatedSelected = YES; }
	}		
	
	if (selectAllUnSelects == YES)
	{ 
		selectAllUnSelects = NO; 
		[relatedSelectAllButton setTitle:@"Select All"];
	}
	else
	{ 
		selectAllUnSelects = YES; 
		[relatedSelectAllButton setTitle:@"Unselect All"];
	}
	
}

#pragma mark "Navigation/Display Methods"

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	LCEntity *selectedEntity = [incident entity];
	if ([selectedEntity object]) 
	{
		NSArray *graphableMetrics = [LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:[selectedEntity object]]];
		if ([(LCMetric *)[selectedEntity metric] isGraphable] && ![graphableMetrics containsObject:[selectedEntity metric]])
		{ 
			LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
			document.initialEntity = [selectedEntity metric];
			[[NSDocumentController sharedDocumentController] addDocument:document];
			[document makeWindowControllers];
			[document showWindows];			
		}
		else
		{ 
			LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
			document.initialEntity = [selectedEntity object];
			[[NSDocumentController sharedDocumentController] addDocument:document];
			[document makeWindowControllers];
			[document showWindows];			
		}
	}
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	LCEntity *selectedEntity = [incident entity];
	if (selectedEntity)
	{ [[[LCBrowser2Controller alloc] initWithEntity:selectedEntity] autorelease]; }
}

- (IBAction) analyseSelectedClicked:(NSMenuItem *)sender
{
	LCEntity *selectedEntity = [incident entity];
	if ([selectedEntity object])
	{ [[LCMetricAnalysisWindowController alloc] initWithObject:[selectedEntity object]]; }
}

- (IBAction) metricHistoryClicked:(NSMenuItem *)sender
{
	LCEntity *selectedEntity = [incident entity];
	if ([selectedEntity metric])
	{ [[LCMetricHistoryWindowController alloc] initWithMetric:[selectedEntity metric]]; }
}

#pragma mark "Action Methods"

- (void) refreshActionTableView
{
	[actionTableView reloadData];
}

- (IBAction) executeActionClicked:(id)sender
{
	/* Get selected */
	if ([[actionArrayController selectedObjects] count] < 1) return;
	LCAction *action = [[actionArrayController selectedObjects] objectAtIndex:0];
	[self setActionBeingExecuted:action];
	
	/* Open sheet */
	[NSApp beginSheet:actionExecSheet
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
	
	/* Execute */
	[action execute];
}

- (IBAction) executeActionCloseClicked:(id)sender
{ 
	[NSApp endSheet:actionExecSheet];
	[actionExecSheet close];
}

- (IBAction) editActionsClicked:(id)sender
{
//	[[LCActionListWindowController alloc] initForCustomer:[[incident entity] customer]];	
}

#pragma mark "Accessor Methods"

@synthesize incident;
@synthesize caseList;
@synthesize incidentList;
@synthesize incidentArraySortDescriptors;
@synthesize relatedIncidentArraySortDescriptors;
@synthesize caseArraySortDescriptors;
@synthesize drawerVisible;
@synthesize actionBeingExecuted;
@synthesize objectTree;

@end
