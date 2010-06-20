//
//  LCBrowserIncidentsContentController.m
//  Lithium Console
//
//  Created by James Wilson on 8/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserIncidentsContentController.h"

#import "LCIncidentController.h"
#import "LCMetricGraphDocument.h"
#import "LCBrowser2Controller.h"
#import "LCMetricHistoryWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCResetTriggerRulesWindowController.h"
#import "LCContainer.h"
#import "LCConsoleController.h"
#import "LCFaultHistoryController.h"
#import "LCActionListWindowController.h"
#import "LCCaseController.h"

@implementation LCBrowserIncidentsContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"IncidentsContent" bundle:nil];
	if (self)
	{
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		[self loadView];
		
		/* Force update of selection */
		[self observeValueForKeyPath:@"selection" ofObject:incidentArrayController change:nil context:nil];
		
		/* Observe future change in selection */		
		[incidentArrayController addObserver:self 
								  forKeyPath:@"selection" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
									 context:nil];
		
		/* Set Inpector Binding */
		[inspectorController bind:@"target" toObject:self withKeyPath:@"selectedIncident" options:nil];
	}
	return self;
}	

- (void) removedFromBrowserWindow
{
	[self clearActionMenu:[[LCConsoleController masterController] executeActionMenu]];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[incidentArrayController removeObserver:self forKeyPath:@"selection"];
	[inspectorController unbind:@"target"];
	[objectTree release];
	[super dealloc];
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == incidentArrayController)
	{
		/* Update selection properties */
		if ([[incidentArrayController selectedObjects] count] > 0)
		{ self.selectedIncident = [[incidentArrayController selectedObjects] objectAtIndex:0]; }
		else
		{ self.selectedIncident = nil; }
		self.selectedIncidents = [incidentArrayController selectedObjects];

		/* Update Object Tree */
		if (self.selectedIncident)
		{
			/* Create new object tree */
			self.objectTree = [[[LCObjectTree alloc] initWithObject:[[self.selectedIncident entity] object]] autorelease];
			[objectOutlineView expandAllItemsUsingPreferences];		
		}	
		else
		{
			self.objectTree = nil; 
		}	
	}
}

#pragma mark "Selection"

@synthesize selectedIncident;
- (void) setSelectedIncident:(LCIncident *)value
{
	[selectedIncident release];
	selectedIncident = [value retain];

	/* Update execute action menus */
	if (selectedIncident)
	{
		[self buildActionMenu:[[LCConsoleController masterController] executeActionMenu] forIncident:selectedIncident];
		[self buildActionMenu:actionMenu forIncident:selectedIncident];
	}
	else 
	{
		[self clearActionMenu:actionMenu];
		[self clearActionMenu:[[LCConsoleController masterController] executeActionMenu]];
	}

}	
	
@synthesize selectedIncidents;

- (void) selectIncident:(LCIncident *)incident
{
	[incidentArrayController setSelectedObjects:[NSArray arrayWithObject:incident]];
	[incidentTableView scrollRowToVisible:[[incidentArrayController arrangedObjects] indexOfObject:incident]];
}

#pragma mark "Browser UI Actions"

- (void) incidentTableViewDoubleClick:(NSArray *)doubleClickedIncidents
{
	/* Called when there is a doubleclick on the incident tableview */
	
	for (LCIncident *incident in doubleClickedIncidents)
	{ [[LCIncidentController alloc] initForIncident:incident]; }
}

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	NSMutableArray *selectedEntities;
	
	/* Get the current selected entities */
	selectedEntities = [incidentArrayController valueForKeyPath:@"selectedObjects.entity"];
	
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

- (IBAction) refreshClicked:(id)sender
{
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		[customer.activeIncidentsList highPriorityRefresh];
		[customer.openCasesList highPriorityRefresh];
	}
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	NSMutableArray *selectedEntities;
	
	/* Get the current selected entities */
	selectedEntities = [incidentArrayController valueForKeyPath:@"selectedObjects.entity"];
	
	/* Open browser for first entity */
	if (selectedEntities && [selectedEntities count] > 0)
	{ [[[LCBrowser2Controller alloc] initWithEntity:[selectedEntities objectAtIndex:0]] autorelease]; }
}

- (IBAction) reviewActiveTriggersClicked:(id)sender
{
	if ([selectedIncidents count] < 1) return;
	LCDevice *dev = [(LCEntity *) [[selectedIncidents objectAtIndex:0] entity] device];
	LCReviewActiveTriggersWindowController *controller = [[LCReviewActiveTriggersWindowController alloc] initForDevice:dev];
	[controller setParentWindow:[browser window]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[browser window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) restartMonitoringProcessClicked:(id)sender
{
	LCIncident *inc;
	for (inc in selectedIncidents)
	{
		[[[inc entity] device] restartMonitoringProcess];
	}
}

- (IBAction) connectUsingSSH:(id)sender
{
	/* Get selected */
	for (LCIncident *inc in selectedIncidents)
	{
		[[inc entity] connectUsingSSH];
	}
}

- (IBAction) connectUsingTelnet:(id)sender
{
	/* Get selected */
	for (LCIncident *inc in selectedIncidents)
	{
		[[inc entity] connectUsingTelnet];
	}
}

- (IBAction) connectUsingWeb:(id)sender
{
	/* Get selected */
	for (LCIncident *inc in selectedIncidents)
	{
		[[inc entity] connectUsingWeb];
	}
}

- (IBAction) connectUsingARD:(id)sender
{
	/* Get selected */
	for (LCIncident *inc in selectedIncidents)
	{
		[[inc entity] connectUsingARD];
	}
}

- (IBAction) copyWebUrlToClipBoardClicked:(id)sender
{
	/* Get selected */
	for (LCIncident *inc in selectedIncidents)
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
	for (LCIncident *inc in selectedIncidents)
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
	for (LCIncident *inc in selectedIncidents)
	{
		[[inc entity] highPriorityRefresh];
	}
}

- (IBAction) faultHistoryClicked:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{
		[[LCMetricHistoryWindowController alloc] initWithMetric:[[inc entity] metric]];
	}
}

- (IBAction) analyseSelectedClicked:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{
		[[LCMetricAnalysisWindowController alloc] initWithObject:[[inc entity] object]];
	}
}

- (IBAction) triggerTuningClicked:(id)sender
{
	/* Get selected */
	if ([selectedIncidents count] < 1) return;
	LCEntity *selectedEntity = (LCEntity *) [[selectedIncidents objectAtIndex:0] entity];
	
	/* Check selected */
	if (!selectedEntity || [[selectedEntity typeInteger] intValue] < 5)
	{ return; }
	
	/* Open trigger tuning window */
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[selectedEntity object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[browser window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) resetTriggerRulesClicked:(id)sender
{
	/* Get selected */
	if ([selectedIncidents count] < 1) return;
	LCEntity *selectedEntity = (LCEntity *) [[selectedIncidents objectAtIndex:0] entity];
	
	LCResetTriggerRulesWindowController *controller = (LCResetTriggerRulesWindowController *) [[LCResetTriggerRulesWindowController alloc] initForEntity:[selectedEntity metric]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[browser window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) viewIncident:(id)sender
{
	for (LCIncident *incident in selectedIncidents)
	{ [[LCIncidentController alloc] initForIncident:incident]; }	
}

- (IBAction) clearIncident:(id)sender
{
	for (LCIncident *incident in selectedIncidents)
	{ [incident clearIncident]; }
}

- (IBAction) refreshIncidentsForSelectedIncident:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{ [[[[inc entity] customer] activeIncidentsList] highPriorityRefresh]; }
}

- (IBAction) browseToSelectedIncident:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{ [[[LCBrowser2Controller alloc] initWithEntity:[inc entity]] autorelease]; }
}

- (IBAction) graphSelectedIncident:(id)sender
{
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
	for (LCIncident *inc in selectedIncidents)
	{ 
		if ([[inc entity] object]) 
		{ [[LCMetricAnalysisWindowController alloc] initWithObject:[[inc entity] object]]; }
	}
}

- (IBAction) faultHistoryForSelectedIncident:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{ 
		[[LCFaultHistoryController alloc] initForEntity:[inc entity]];
	}		
}

- (IBAction) metricHistoryForSelectedIncident:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{ 
		if ([[inc entity] metric]) [[LCMetricHistoryWindowController alloc] initWithMetric:[[inc entity] metric]];
	}	
}

- (IBAction) triggerTuningForSelectedIncident:(id)sender
{
	for (LCIncident *inc in selectedIncidents)
	{ 
		LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[[inc entity] object]];
		[NSApp beginSheet:[controller window] 
		   modalForWindow:[[self view] window] 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];			
	}
}

- (IBAction) editActionsForSelectedIncident:(id)sender
{
//	if (selectedIncidents.count < 1)
//	{ return; }
//	LCIncident *inc = [selectedIncidents objectAtIndex:0];
//	[[LCActionListWindowController alloc] initForCustomer:[[inc entity] customer]];
}

- (IBAction) cancelPendingActionsClicked:(id)sender
{
	if (selectedIncidents.count < 1)
	{ return; }
	
	/* Sort array by customer name */
	NSArray *descriptors = [NSArray arrayWithObject:[[NSSortDescriptor alloc] initWithKey:@"customerName" ascending:YES]];
	NSArray *sortedIncidents = [selectedIncidents sortedArrayUsingDescriptors:descriptors];
	
	/* Loop through incidents */
	NSString *lastCustName = nil;
	NSMutableArray *currentArray = [NSMutableArray array];
	for (LCIncident *inc in sortedIncidents)
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

- (IBAction) openCaseForSelectedClicked:(id)sender
{
	/* Open a new case for the selected incident(s) */
	NSMutableArray *entarray = [NSMutableArray array];
	
	/* Check something is selected */
	if (selectedIncidents.count < 1) 
	{ return; }
	
	/* Loop through indexes */
	NSString *prev_custname = nil;
	for (LCIncident *incident in selectedIncidents)
	{
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

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	/* Obtain selection */
	SEL action = [item action];
	
	/* Reference Customer */
	LCCustomer *referenceCustomer = nil;
	if (selectedIncident)
	{ referenceCustomer = [[selectedIncident entity] customer]; }
	
	/*
	 * Lithium Menu
	 */
	
	if (action == @selector(refreshClicked:))
	{ return YES; }
	
	if (action == @selector(refreshIncidentsClicked:) || action == @selector(refreshCasesClicked:))
	{ return YES; }
	
	/* 
	 * Device Device
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
	
	if (action == @selector(newCaseClicked:))
	{ return YES; }	

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
	
	if (action == @selector(refreshIncidentsClicked:))
	{ if (selectedIncident) return YES; }
	
	return NO;
}

#pragma mark "Action Menu Methods"

- (void) buildActionMenu:(NSMenu *)menu forIncident:(LCIncident *)incident
{
	/* Clear old */
	[self clearActionMenu:menu];
	
	/* Loop through actions */
	for (LCAction *action in incident.actions)
	{
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:[action desc]
													  action:@selector(execute)
											   keyEquivalent:@""];
		if ([[[incident entity] customer] userIsNormal]) [item setTarget:action];
		[menu insertItem:item atIndex:[[menu itemArray] count]];
		[item autorelease];
	}	
}

- (void) clearActionMenu:(NSMenu *)menu
{
	for (NSMenuItem *item in [menu itemArray])
	{ [menu removeItem:item]; }	
}

#pragma mark "Properties"

- (CGFloat) preferredFixedComponentHeight
{ return 200.0; }

- (CGFloat) preferredInspectorWidth
{ return 210.0; }

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@synthesize browser;

@synthesize objectTree;

@end
