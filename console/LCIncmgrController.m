//
//  LCIncmgrController.m
//  Lithium Console
//
//  Created by James Wilson on 27/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncmgrController.h"
#import "LCCustomer.h"
#import "LCCustomerList.h"
#import "LCIncident.h"
#import "LCCase.h"
#import "LCIncidentList.h"
#import "LCIncidentController.h"
#import "LCCaseList.h"
#import "LCCaseController.h"
#import "LCMetricGraphDocument.h"
#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCConsoleController.h"
#import "LCActionListWindowController.h"
#import "LCLithiumSetupWindowController.h"
#import "LCReviewActiveTriggersWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCResetTriggerRulesWindowController.h"

@implementation LCIncmgrController

#pragma mark "Initialisation"

- (LCIncmgrController *) init
{
	int i;
	NSMutableDictionary *dict;

	/* 
	 * Incident Filter Predicates 
	 */
	
	/* Create array */
	incidentFilterPredicates = [[NSMutableArray array] retain];
	
	/* Unhandled Filter */
	dict = [NSMutableDictionary dictionary];
	[incidentFilterPredicates addObject:dict];
	[dict setObject:[NSPredicate predicateWithFormat:@"caseID == 0"] forKey:@"predicate"];
	[dict setObject:@"Unhandled Incidents" forKey:@"desc"];
	
	/* All Incidents (nil) filter */
	dict = [NSMutableDictionary dictionary];
	[incidentFilterPredicates addObject:dict];
	[dict setObject:@"All Incidents" forKey:@"desc"];	

	/* Search string filter */
	incidentCustomSearchDictionary = [NSMutableDictionary dictionary];
	[incidentFilterPredicates addObject:incidentCustomSearchDictionary];
	[incidentCustomSearchDictionary setObject:@"Custom Search" forKey:@"desc"];	
	
	/* 
	 * Case Filter Predicates 
	 */
	
	/* Create array */
	caseFilterPredicates = [[NSMutableArray array] retain];
	
	/* Self-owned filter */
	dict = [NSMutableDictionary dictionary];
	[caseFilterPredicates addObject:dict];
	NSMutableString *filterString = [NSMutableString string];
	int custCount = 0;
	for (i=0; i < [[LCCustomerList masterArray] count]; i++)
	{
		LCCustomer *cust = [[LCCustomerList masterArray] objectAtIndex:i];
		if (![cust disabled])
		{
			LCAuthenticator *auth = [LCAuthenticator authForCustomer:cust];
			if (custCount > 0) [filterString appendString:@" OR "];
			[filterString appendFormat:@"(customer.name == \"%@\" AND owner == \"%@\")", [cust name], [auth username]]; 
			custCount++;
		}
	}
	if ([filterString length] > 0)
	{ [dict setObject:[NSPredicate predicateWithFormat:filterString] forKey:@"predicate"]; }
	[dict setObject:@"Cases I Own" forKey:@"desc"];
	
	/* All Cases (nil) filter */
	dict = [NSMutableDictionary dictionary];
	[caseFilterPredicates addObject:dict];
	[dict setObject:@"All Open Cases" forKey:@"desc"];
	
	/* Search string filter */
	caseCustomSearchDictionary = [NSMutableDictionary dictionary];
	[caseFilterPredicates addObject:caseCustomSearchDictionary];
	[caseCustomSearchDictionary setObject:@"Custom Search" forKey:@"desc"];
	
	/*
	 * NIB Setup
	 */
		
	/* Load NIB */
	[self setShouldCascadeWindows:NO];
	[super initWithWindowNibName:@"IncidentManager"];
	[self window];
	[self buildToolbar];
	
	/* Set sort descriptors */
	[self setCaseArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"openDateShortString" ascending:NO] autorelease]]];
	[self setIncidentArraySortDescriptors:[NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"startDateShortString" ascending:YES] autorelease]]];
	
	/* 
	 * Incident view refresh 
	 */
	incidentRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:10.0 target:inc_tview
														  selector:@selector(reloadData) userInfo:nil repeats:YES];
	
	/* Add to persistent window list */
	[[LCConsoleController masterController] insertObject:self inPersistentWindowsAtIndex:0];

	/* Show window */
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[toolbar release];
	[tb_items release];
	[tb_selitems release];
	[tb_defitems release];
	[incidentFilterPredicates release];
	[caseFilterPredicates release];
	[self setIncidentArraySortDescriptors:nil];
	[self setCaseArraySortDescriptors:nil];
	[searchString release];
	[incidentRefreshTimer invalidate];
	
	[caseArraySortDescriptors release];
	[incidentArraySortDescriptors release];
	[super dealloc];
}

#pragma mark "Encoding"

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:NSStringFromRect([[self window] frame]) forKey:@"frame"];
	[encoder encodeObject:[splitView frameStrings] forKey:@"splitViewFrameSizes"];
	[encoder encodeInt:[caseFilterArrayController selectionIndex] forKey:@"caseFilterSelectionIndex"];
	[encoder encodeInt:[incidentFilterArrayControler selectionIndex] forKey:@"incidentFilterSelectionIndex"];
	if ([toolbar isVisible]) [encoder encodeBool:YES forKey:@"toolbarIsVisible"];
	else [encoder encodeBool:NO forKey:@"toolbarIsVisible"];
	if ([self searchString]) [encoder encodeObject:[self searchString] forKey:@"searchString"];
}

- (LCIncmgrController *) initWithCoder:(NSCoder *)decoder
{
	[self init];

	if ([decoder decodeObjectForKey:@"frame"])
	{ [[self window] setFrame:NSRectFromString([decoder decodeObjectForKey:@"frame"]) display:YES]; }
	
	if ([decoder decodeObjectForKey:@"splitViewFrameSizes"])
	{ [splitView setFrameStrings:[decoder decodeObjectForKey:@"splitViewFrameSizes"]]; }
	
	[caseFilterArrayController setSelectionIndex:[decoder decodeIntForKey:@"caseFilterSelectionIndex"]];
	[incidentFilterArrayControler setSelectionIndex:[decoder decodeIntForKey:@"incidentFilterSelectionIndex"]];
	
	if ([decoder decodeBoolForKey:@"toolbarIsVisible"] == NO)
	{ [[self window] toggleToolbarShown:self]; }
	
	if ([decoder decodeObjectForKey:@"searchString"])
	{ [self setSearchString:[decoder decodeObjectForKey:@"searchString"]]; }
    
	return self;
}

#pragma mark "Case Action Methods"

- (IBAction) viewCaseClicked:(id)sender
{
	NSArray *selectedCases = [casesArrayController selectedObjects];
	for (id loopItem in selectedCases)
	{ [[LCCaseController alloc] initWithCase:loopItem]; }
	
}

- (IBAction) newCaseClicked:(id)sender
{
	[[LCCaseController alloc] initForNewCase];
}

- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender
{
	/* Open a new case for the selected incident(s) */
	int i;
	NSMutableArray *entarray = [NSMutableArray array];

	/* Check something is selected */
	if ([[incidentsArrayController selectedObjects] count] < 1) 
	{ return; }
	
	/* Loop through indexes */
	NSString *prev_custname = nil;
	for(i = 0; i < [[incidentsArrayController selectedObjects] count]; i++)
	{
		LCIncident *incident = [[incidentsArrayController selectedObjects] objectAtIndex:i];
		LCEntityDescriptor *entityDescriptor = [incident entityDescriptor];
		[entarray insertObject:[entityDescriptor locateEntity:YES] atIndex:[entarray count]];
		
		/* Check customer consistency */
		if (prev_custname && [[entityDescriptor cust_name] isEqualToString:prev_custname] == NO)
		{ 
			/* Customer mis-match, cancel operation */
			NSAlert *alert = [[NSAlert alloc] init];
			[alert addButtonWithTitle:@"OK"];
			[alert setMessageText:@"Error: Incidents from multiple customers selected"];
			[alert setInformativeText:@"Cases can only be opened for multiple incidents where all incidents belong to a single customer."];
			[alert setAlertStyle:NSCriticalAlertStyle];
			[alert runModal];
			[alert autorelease];
			return;
		}
		prev_custname = [entityDescriptor cust_name];
	}
	
	/* Open case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:(NSArray *)entarray];
}

- (IBAction) closeCaseClicked:(id)sender
{
	/* Close the selected cases */
	
	/* Check cases are selected */
	if ([[case_tview selectedRowIndexes] count] < 1) return;
	
	/* Call updateSelectedCases to close */
	[self updateSelectedCases:YES];
}

- (IBAction) updateCaseClicked:(id)sender
{
	/* Update the selected cases */

	/* Check cases are selected */
	if ([[case_tview selectedRowIndexes] count] < 1) return;
	
	/* Call updateSelectedCases to update */
	[self updateSelectedCases:NO];
}

- (void) updateSelectedCases:(BOOL)close_case
{
	logop_closecase = close_case;
	
	[l_entry setString:@""];
	[l_timefield setStringValue:@""]; 
	[NSApp beginSheet: l_window
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) logRecordClicked:(id)sender
{
	/* Called when 'Record' is clicked on log entries */
	int i;
	
	/* Loop through indexes */
	logop_count = 0; 
	for(i = 0; i < [[casesArrayController selectedObjects] count]; i++)
	{
		LCCase *selectedCase = [[casesArrayController selectedObjects] objectAtIndex:i];
		/* Check if the intended action is to close or update the case */
		if (logop_closecase == YES)
		{
			/* Close selected case */
			[selectedCase closeCaseWithFinalLogEntry:[l_entry string] delegate:self];
		}
		else
		{
			/* Update selected case */
			LCCaseLogEntry *log = [[LCCaseLogEntry alloc] initWithString:[l_entry string] cas:selectedCase timespent:[[l_timefield stringValue] floatValue]];
			[log setDelegate:self];
			[log record:selectedCase];
		}
		logop_count++;
	}

	if (logop_count > 0)
	{
		if (logop_closecase == YES) [l_infofield setStringValue:@"Closing cases..."];
		if (logop_closecase == NO) [l_infofield setStringValue:@"Recording log entries..."];
	}
	
	/* Check state */
	if (logop_count > 0)
	{ 
		/* Refresh in progress */
		[l_recordbtn setEnabled:NO];
		[l_cancelbtn setEnabled:NO];
		[l_pbar startAnimation:self];
		[l_infofield setHidden:NO];
	}
	else
	{
		/* No refresh */
		[self endLogEntrySheet];
	}
}

- (void) caseClosedFinished:(LCCase *)sender
{
	logop_count--;

	if (logop_count == 0)
	{
		/* All cases are closed */
		[self endLogEntrySheet]; 
	}
}

- (void) logEntryRecordFinished:(LCCaseLogEntry *)sender
{
	logop_count--;
	
	if (logop_count == 0)
	{ 
		/* All cases have been updated */
		[self endLogEntrySheet]; 
	}
}

- (IBAction) logCancelClicked:(id)sender
{
	/* Cancel recording */
	[self endLogEntrySheet];
}

-(void) endLogEntrySheet
{
	[l_recordbtn setEnabled:YES];
	[l_cancelbtn setEnabled:YES];
	[l_pbar stopAnimation:self];
	[l_infofield setHidden:YES];
	[NSApp endSheet:l_window];	
	[l_window close];
}

#pragma mark "Incident Action Methods"

- (IBAction) viewIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (id loopItem in selectedIncidents)
	{ [[LCIncidentController alloc] initForIncident:loopItem]; }	
}

- (IBAction) clearIncident:(id)sender
{
	NSEnumerator *selectedEnum = [[incidentsArrayController selectedObjects] objectEnumerator];
	LCIncident *incident;
	while (incident=[selectedEnum nextObject])
	{ [incident clearIncident]; }
}

- (IBAction) refreshIncidentsForSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		[[[[inc entity] customer] activeIncidentsList] highPriorityRefresh];
	}
}

- (IBAction) browseToSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		[[[LCBrowser2Controller alloc] initWithEntity:[inc entity]] autorelease];
	}
}

- (IBAction) graphSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		if ([[inc entity] metric]) 
		{
			LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
			document.initialEntity = [[inc entity] metric];
			[[NSDocumentController sharedDocumentController] addDocument:document];
			[document makeWindowControllers];
			[document showWindows];			
		}
	}
}

- (IBAction) metricAnalyseSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		if ([[inc entity] object]) 
		{ [[LCMetricAnalysisWindowController alloc] initWithObject:[[inc entity] object]]; }
	}
}

- (IBAction) faultHistoryForSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		[[LCFaultHistoryController alloc] initForEntity:[inc entity]];
	}		
}

- (IBAction) metricHistoryForSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		if ([[inc entity] metric]) [[LCMetricHistoryWindowController alloc] initWithMetric:[[inc entity] metric]];
	}	
}

- (IBAction) triggerTuningForSelectedIncident:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	for (LCIncident *inc in selectedIncidents)
	{ 
		LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[[inc entity] object]];
		[NSApp beginSheet:[controller window] 
		   modalForWindow:[self window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}
}

- (IBAction) editActionsForSelectedIncident:(id)sender
{
//	if ([[incidentsArrayController selectedObjects] count] < 1)
//	{ return; }
//	LCIncident *inc = [[incidentsArrayController selectedObjects] objectAtIndex:0];
//	[[LCActionListWindowController alloc] initForCustomer:[[inc entity] customer]];
}

- (IBAction) cancelPendingActionsClicked:(id)sender
{
	if ([[incidentsArrayController selectedObjects] count] < 1)
	{ return; }
	
	/* Sort array by customer name */
	NSArray *descriptors = [NSArray arrayWithObject:[[NSSortDescriptor alloc] initWithKey:@"customerName" ascending:YES]];
	NSArray *sortedIncidents = [[incidentsArrayController selectedObjects] sortedArrayUsingDescriptors:descriptors];
	
	/* Loop through incidents */
	LCIncident *inc;
	NSString *lastCustName = nil;
	NSMutableArray *currentArray = [NSMutableArray array];
	for (inc in sortedIncidents)
	{
		/* Check customer name consistency */
		if (lastCustName && ![lastCustName isEqualToString:[inc customerName]] && [currentArray count] > 0)
		{
			/* Send cancel pending request */
			LCIncident *primeIncident = [currentArray objectAtIndex:0];
			[primeIncident cancelPendingActionsForIncidents:currentArray];
			[currentArray removeAllObjects];
		}
		
		/* Add to array */
		[currentArray addObject:inc];
		lastCustName = [NSString stringWithString:[inc customerName]];
	}
	if ([currentArray count] > 0)
	{
		/* Send cancel pending request */
		LCIncident *primeIncident = [currentArray objectAtIndex:0];
		[primeIncident cancelPendingActionsForIncidents:currentArray];
	}
}

#pragma mark "Customer Action Methods"

- (IBAction) refreshIncidentsClicked:(id)sender
{
	LCCustomer *cust = [[customersArrayController selectedObjects] objectAtIndex:0];
	
	[[cust activeIncidentsList] highPriorityRefresh];
}

- (IBAction) refreshCasesClicked:(id)sender
{
	LCCustomer *cust = [[customersArrayController selectedObjects] objectAtIndex:0];

	[[cust openCasesList] highPriorityRefresh];
}

#pragma mark "Table View Methods"

- (void) caseTableViewDoubleClick:(NSArray *)selectedCases
{
	/* Called when there is a doubleclick on the case tableview */
	for (id loopItem in selectedCases)
	{ 
		[[LCCaseController alloc] initWithCase:loopItem]; 
	}
}

- (void) incidentTableViewDoubleClick:(NSArray *)selectedIncidents
{
	/* Called when there is a doubleclick on the incident tableview */
	
	for (id loopItem in selectedIncidents)
	{ [[LCIncidentController alloc] initForIncident:loopItem]; }
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	/* Obtain selection */
	SEL action = [item action];
	
	/* Incident */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *selectedIncident = nil;
	if ([selectedIncidents count] > 0 && [[self window] firstResponder] == inc_tview) 
	{ selectedIncident = [selectedIncidents objectAtIndex:0]; }
	
	/* Cases */
	NSArray *selectedCases = [casesArrayController selectedObjects];
	LCCase *selectedCase = nil;
	if ([selectedCases count] > 0 && [[self window] firstResponder] == case_tview)
	{ selectedCase = [selectedCases objectAtIndex:0]; }
	
	/* Customers */
	NSArray *selectedCustomers = [customersArrayController selectedObjects];
	LCCustomer *selectedCustomer = nil;
	if ([selectedCustomers count] > 0 && [[self window] firstResponder] == customerTableView)
	{ selectedCustomer = [selectedCustomers objectAtIndex:0]; }
	
	/* Reference Customer */
	LCCustomer *referenceCustomer = nil;
	if (selectedCustomer) 
	{ referenceCustomer = selectedCustomer; }
	else if (selectedIncident)
	{ referenceCustomer = [[selectedIncident entity] customer]; }
	else if (selectedCase)
	{ referenceCustomer = [selectedCase customer]; }
	
	/* Toolbar */
	
	/*
	 * Lithium 
	 */

	if (action == @selector(refreshClicked:))
	{ return YES; }
	
	if (action == @selector(refreshIncidentsClicked:) || action == @selector(refreshCasesClicked:))
	{ if (selectedCase || selectedIncident || selectedCustomer) return YES; }
	
	if (action == @selector(lithiumSetupClicked:))
	{ if ((selectedCase || selectedIncident || selectedCustomer) && [referenceCustomer userIsAdmin]) return YES; }
	
	/* 
	 * Device
	 */
	
	if (action == @selector(restartMonitoringProcessClicked:))
	{ if (selectedIncident && [referenceCustomer userIsAdmin]) return YES; }

	if (action == @selector(reviewActiveTriggersClicked:))
	{ if (selectedIncident && [referenceCustomer userIsAdmin]) return YES; }

	if (action == @selector(resetDeviceTriggerRulesClicked:))
	{ if (selectedIncident && [referenceCustomer userIsAdmin]) return YES; }

	if ([NSStringFromSelector(action) hasPrefix:@"connectUsing"])
	{ if (selectedIncident) return YES; }
	
	/* 
	 * Monitored Enity Menu 
	 */
	
	if (action == @selector(refreshEntityClicked:))
	{ if (selectedIncident) return YES; }

	if (action == @selector(graphSelectedClicked:))
	{ if (selectedIncident) return YES; }

	if (action == @selector(analyseSelectedClicked:))
	{ if (selectedIncident) return YES; }

	if (action == @selector(faultHistoryClicked:))
	{ if (selectedIncident) return YES; }

	if (action == @selector(browseToSelectedClicked:))
	{ if (selectedIncident) return YES; }

	if (action == @selector(triggerTuningClicked:))
	{ if (selectedIncident && [referenceCustomer userIsAdmin]) return YES; }

	if (action == @selector(resetTriggerRulesClicked:))
	{ if (selectedIncident && [referenceCustomer userIsAdmin]) return YES; }

	if (action == @selector(openCaseForSelectedClicked:))
	{ if (selectedIncident && [referenceCustomer userIsNormal]) return YES; }	

	if (action == @selector(copyWebUrlToClipBoardClicked:))
	{ if (selectedIncident) return YES; }	
	
	if (action == @selector(openWebUrlInBrowserClicked:))
	{ if (selectedIncident) return YES; }	
	
	/*
	 * Incident 
	 */

	if (action == @selector(viewIncident:))
	{ if (selectedIncident) return YES; }
	
	if (action == @selector(editActionsForSelectedIncident:))
	{ if (![referenceCustomer userIsAdmin]) return NO; }

	if (action == @selector(triggerTuningForSelectedIncident:))
	{ if (![referenceCustomer userIsAdmin]) return NO; }
	
	if ([NSStringFromSelector(action) hasSuffix:@"Incident:"])
	{ if (selectedIncident && [referenceCustomer userIsNormal]) return YES; }
	
	if (action == @selector(cancelPendingActionsClicked:))
	{ if (selectedIncident && [referenceCustomer userIsNormal]) return YES; }

	if (action == @selector(refreshIncidentsClicked:) || action == @selector(refreshCasesClicked:))
	{ if (selectedCase || selectedIncident) return YES; }
	
	/*
	 * Case
	 */
	
	if (action == @selector(viewCaseClicked:))
	{ if (selectedCase) return YES; }

	if (action == @selector(closeCaseClicked:))
	{ if (selectedCase && [referenceCustomer userIsNormal]) return YES; }
	
	/*
	 * Update execute action menu
	 */

	{
		NSMenu *menu = [[LCConsoleController masterController] executeActionMenu];
		
		/* Clear old */
		NSEnumerator *itemEnum = [[menu itemArray] objectEnumerator];
		NSMenuItem *item;
		while (item=[itemEnum nextObject])
		{ [menu removeItem:item]; }
		
		/* Get selected */
		NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
		LCIncident *selectedIncident = nil;
		if ([selectedIncidents count] > 0 && [[self window] firstResponder] == inc_tview) 
		{ selectedIncident = [selectedIncidents objectAtIndex:0]; }
		
		/* Loop through actions */
		NSEnumerator *actionEnum = [[selectedIncident actions] objectEnumerator];
		LCAction *action;
		while (action=[actionEnum nextObject])
		{
			NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:[action desc]
														  action:@selector(execute)
												   keyEquivalent:@""];
			if ([[[selectedIncident entity] customer] userIsNormal]) [item setTarget:action];
			[menu insertItem:item atIndex:[[menu itemArray] count]];
		}	
	}
	
	return NO;
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"IncmgrToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	tb_items = [[NSMutableDictionary dictionary] retain];	
	tb_selitems = [[NSMutableArray array] retain];
	tb_defitems = [[NSMutableArray array] retain];
	
	/* Refresh */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];
	
	/* New case for incident item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"New Case for Incident"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (openCaseForSelectedClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseOpenForSingle.tiff"]];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];
	
	/* Close case item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Close Case"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (closeCaseClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseClose.tiff"]];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];	
	
	/* Incident View Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Filter Incidents"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:filterIncidentsView];
	[tbitem setMinSize:[filterIncidentsView bounds].size];
	[tbitem setMaxSize:[filterIncidentsView bounds].size];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];	
		
	/* Case View Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Filter Cases"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:filterCasesView];
	[tbitem setMinSize:[filterCasesView bounds].size];
	[tbitem setMaxSize:[filterCasesView bounds].size];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];	

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];	
	
	/* Search */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Search"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:searchView];
	[tbitem setMinSize:[searchView bounds].size];
	[tbitem setMaxSize:[searchView bounds].size];
	[tb_items setObject:tbitem forKey:[tbitem itemIdentifier]];
	[tb_defitems insertObject:[tbitem itemIdentifier] atIndex:[tb_defitems count]];		
	
	[[self window] setToolbar:toolbar];
}

- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag 
{ return [tb_items objectForKey: identifier]; }

- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar 
{ return tb_defitems; }

- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar 
{ return tb_defitems; }

- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar 
{ return tb_selitems; }

- (IBAction) toggleToolbarClicked:(id)sender
{ [[self window] toggleToolbarShown:sender]; }

#pragma mark "Navigation/Display Methods"

- (IBAction) refreshClicked:(id)sender
{
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[customer.activeIncidentsList highPriorityRefresh];
		[customer.openCasesList highPriorityRefresh];
	}
}

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	NSMutableArray *selectedEntities = [NSMutableArray array];
	
	/* Get the current selected entities */
	if ([[self window] firstResponder] == inc_tview)
	{
		/* Incident is selected */
		selectedEntities = [incidentsArrayController valueForKeyPath:@"selectedObjects.entity"];
	}
	else if ([[self window] firstResponder] == case_tview)
	{
		/* Case is selected */
		selectedEntities = [casesArrayController valueForKeyPath:@"selectedObjects.@unionOfArrays.entityList.entities"];
	}
	
	/* Open graph */
	if (selectedEntities && [selectedEntities count] > 0)
	{
		LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
		document.initialEntities = selectedEntities;
		[[NSDocumentController sharedDocumentController] addDocument:document];
		[document makeWindowControllers];
		[document showWindows];			
	}
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	NSMutableArray *selectedEntities = [NSMutableArray array];
	
	/* Get the current selected entities */
	if ([[self window] firstResponder] == inc_tview)
	{
		/* Incident is selected */
		selectedEntities = [incidentsArrayController valueForKeyPath:@"selectedObjects.entity"];
	}
	else if ([[self window] firstResponder] == case_tview)
	{
		/* Case is selected */
		selectedEntities = [casesArrayController valueForKeyPath:@"selectedObjects.@unionOfArrays.entityList.entities"];
	}
	
	/* Open browser for first entity */
	if (selectedEntities && [selectedEntities count] > 0)
	{ [[[LCBrowser2Controller alloc] initWithEntity:[selectedEntities objectAtIndex:0]] autorelease]; }
}

#pragma mark "Shared UI Actions"

- (IBAction) lithiumSetupClicked:(id)sender
{ 
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	if ([selectedIncidents count] < 1) return;
	[[LCLithiumSetupWindowController alloc] initForCustomer:[(LCEntity *)[[selectedIncidents objectAtIndex:0] entity] customer] showingTab:nil];
}

- (IBAction) reviewActiveTriggersClicked:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	if ([selectedIncidents count] < 1) return;
	LCDevice *dev = [(LCEntity *) [[selectedIncidents objectAtIndex:0] entity] device];
	LCReviewActiveTriggersWindowController *controller = [[LCReviewActiveTriggersWindowController alloc] initForDevice:dev];
	[controller setParentWindow:[self window]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) restartMonitoringProcessClicked:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[[inc entity] device] restartMonitoringProcess];
	}
}

- (IBAction) connectUsingSSH:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[inc entity] connectUsingSSH];
	}
}

- (IBAction) connectUsingTelnet:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[inc entity] connectUsingTelnet];
	}
}

- (IBAction) connectUsingWeb:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[inc entity] connectUsingWeb];
	}
}

- (IBAction) connectUsingARD:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[inc entity] connectUsingARD];
	}
}

- (IBAction) copyWebUrlToClipBoardClicked:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		/* Copy */
		if ([[[inc entity] metric] webURLString])
		{
			NSPasteboard *pb = [NSPasteboard generalPasteboard];
			NSArray *types = [NSArray arrayWithObject:NSStringPboardType];
			[pb declareTypes:types owner:self];
			[pb setString:[[inc entity] metric] forType:NSStringPboardType];
		}
	}
}

- (IBAction) openWebUrlInBrowserClicked:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		/* Open */
		if ([[[inc entity] metric] webURLString])
		{
			NSURL *url = [NSURL URLWithString:[[[inc entity] metric] webURLString]];
			[[NSWorkspace sharedWorkspace] openURL:url];
		}
	}
}

- (IBAction) refreshEntityClicked:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[inc entity] highPriorityRefresh];
	}
}

- (IBAction) faultHistoryClicked:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[LCMetricHistoryWindowController alloc] initWithMetric:[[inc entity] metric]];
	}
}

- (IBAction) analyseSelectedClicked:(id)sender
{
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[LCMetricAnalysisWindowController alloc] initWithObject:[[inc entity] object]];
	}
}

- (IBAction) triggerTuningClicked:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	if ([selectedIncidents count] < 1) return;
	LCEntity *selectedEntity = (LCEntity *) [[selectedIncidents objectAtIndex:0] entity];
	
	/* Check selected */
	if (!selectedEntity || [[selectedEntity typeInteger] intValue] < 5)
	{ return; }
	
	/* Open trigger tuning window */
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[selectedEntity object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) resetTriggerRulesClicked:(id)sender
{
	/* Get selected */
	NSArray *selectedIncidents = [incidentsArrayController selectedObjects];
	if ([selectedIncidents count] < 1) return;
	LCEntity *selectedEntity = (LCEntity *) [[selectedIncidents objectAtIndex:0] entity];
	
	LCResetTriggerRulesWindowController *controller = (LCResetTriggerRulesWindowController *) [[LCResetTriggerRulesWindowController alloc] initForEntity:[selectedEntity metric]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Unbind controllerAlias */
	[controllerAlias setContent:nil];
	[[LCConsoleController masterController] removePersistentWindow:self];
}

#pragma mark "Accessor Methods"

- (NSArray *) customers { return [LCCustomerList masterArray]; }

- (NSArray *) incidentArraySortDescriptors { return incidentArraySortDescriptors; }
- (void) setIncidentArraySortDescriptors:(NSArray *)array
{
	if (incidentArraySortDescriptors) [incidentArraySortDescriptors release];
	incidentArraySortDescriptors = [array retain];
}

- (NSArray *) caseArraySortDescriptors { return caseArraySortDescriptors; }
- (void) setCaseArraySortDescriptors:(NSArray *)array
{
	if (caseArraySortDescriptors) [caseArraySortDescriptors release];
	caseArraySortDescriptors = [array retain];
}

- (NSMutableArray *) incidentFilterPredicates
{ return incidentFilterPredicates; }

- (NSMutableArray *) caseFilterPredicates
{ return caseFilterPredicates; }

- (NSString *) searchString
{ return searchString; }

- (void) setSearchString:(NSString *)string
{ 
	if (searchString) 
	{
		[searchString release];
		searchString = nil;
	}
	
	if (string)
	{
		/* Filter string set */
		searchString = [string copy];
		
		/* Store current selection index */
		if ([caseFilterArrayController selectionIndex] != [caseFilterPredicates indexOfObject:caseCustomSearchDictionary])
		{ prevCaseFilterIndex = [caseFilterArrayController selectionIndex]; }
		if ([incidentFilterArrayControler selectionIndex] != [incidentFilterPredicates indexOfObject:incidentCustomSearchDictionary])
		{ prevIncidentFilterIndex = [incidentFilterArrayControler selectionIndex]; }
		
		/* Create filter predicate */
		NSPredicate *predicate = [NSPredicate predicateWithFormat:@"customer.name CONTAINS[cd] %@ OR customer.desc CONTAINS[cd] %@ OR caseID CONTAINS[cd] %@ OR headline CONTAINS[cd] %@ OR owner CONTAINS[cd] %@ OR requester CONTAINS[cd] %@",
																		string, string, string, string, string, string];
		[caseCustomSearchDictionary setObject:predicate forKey:@"predicate"];
		predicate = [NSPredicate predicateWithFormat:@"incidentID CONTAINS[cd] %@ OR caseID CONTAINS[cd] %@ OR entity.customer.name CONTAINS[cd] %@ OR entity.customer.desc CONTAINS[cd] %@ OR entity.site.name CONTAINS[cd] %@ OR entity.site.desc CONTAINS[cd] %@ OR entity.site.suburb CONTAINS[cd] %@ OR entity.device.name CONTAINS[cd] %@ OR entity.device.desc CONTAINS[cd] %@ OR entity.container.name CONTAINS[cd] %@ OR entity.container.desc CONTAINS[cd] %@ OR entity.object.name CONTAINS[cd] %@ OR entity.object.desc CONTAINS[cd] %@ OR entity.metric.name CONTAINS[cd] %@ OR entity.metric.desc CONTAINS[cd] %@",
															string, string, string, string, string, string, string, string,
															string, string, string, string, string, string, string];
		[incidentCustomSearchDictionary setObject:predicate forKey:@"predicate"];
		
		/* Set custom search as selected */
		[caseFilterArrayController setSelectionIndex:[caseFilterPredicates indexOfObject:caseCustomSearchDictionary]];
		[incidentFilterArrayControler setSelectionIndex:[incidentFilterPredicates indexOfObject:incidentCustomSearchDictionary]];
		
		/* Disable Popups */
		[caseFilterPopUp setEnabled:NO];
		[incidentFilterPopUp setEnabled:NO];
	}
	else
	{
		/* Restore filter selection */
		[caseFilterArrayController setSelectionIndex:prevCaseFilterIndex];
		[incidentFilterArrayControler setSelectionIndex:prevIncidentFilterIndex];
		
		/* Re-enable popups */
		[caseFilterPopUp setEnabled:YES];
		[incidentFilterPopUp setEnabled:YES];
	}
}

@synthesize controllerAlias;
@synthesize customersArrayController;
@synthesize incidentsArrayController;
@synthesize casesArrayController;
@synthesize inc_tview;
@synthesize case_tview;
@synthesize p_window;
@synthesize p_bar;
@synthesize spin;
@synthesize p_window_active;
@synthesize filterIncidentsView;
@synthesize filterCasesView;
@synthesize searchView;
@synthesize splitView;
@synthesize customerTableView;
@synthesize toolbar;
@synthesize tb_items;
@synthesize tb_selitems;
@synthesize tb_defitems;
@synthesize refinc_item;
@synthesize refcase_item;
@synthesize closecase_item;
@synthesize l_window;
@synthesize l_entry;
@synthesize l_timefield;
@synthesize l_pbar;
@synthesize l_infofield;
@synthesize l_recordbtn;
@synthesize l_cancelbtn;
@synthesize incidentFilterPredicates;
@synthesize caseFilterPredicates;
@synthesize incidentFilterArrayControler;
@synthesize caseFilterArrayController;
@synthesize incidentCustomSearchDictionary;
@synthesize caseCustomSearchDictionary;
@synthesize incidentFilterPopUp;
@synthesize caseFilterPopUp;
@synthesize prevCaseFilterIndex;
@synthesize prevIncidentFilterIndex;
@synthesize searchField;
@synthesize incidentRefreshTimer;
@synthesize logop_count;
@synthesize logop_closecase;
@end
