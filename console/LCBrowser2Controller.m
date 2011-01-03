//
//  LCBrowser2Controller.m
//  Lithium Console
//
//  Created by James Wilson on 10/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"
#import "LCCaseController.h"
#import "LCPropertiesViewerController.h"
#import "LCMetricGraphDocument.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCXRDeviceView.h"
#import "LCXSDeviceView.h"
#import "LCWAPView.h"
#import "LCConsoleController.h"
#import "LCIncidentController.h"
#import "LCActionListWindowController.h"
#import "LCActionScriptManagerController.h"
#import "LCServiceEditWindowController.h"
#import "LCServiceScriptManagerController.h"
#import "LCBlankDeviceView.h"
#import "LCBrowserTreeDevicesRoot.h"
#import "LCDeviceEditController.h"
#import "LCLithiumSetupWindowController.h"
#import "LCIncmgrController.h"
#import "LCCaseSearchController.h"
#import "LCUPSDeviceView.h"
#import "LCResetTriggerRulesWindowController.h"
#import "LCAssistController.h"
#import "LCPreferencesController.h"
#import "LCErrorLogWindowController.h"
#import "LCActionEditWindowController.h"
#import "LCMetric.h"
#import "LCSiteEditController.h"
#import "LCDeviceEditController.h"
#import "LCBrowserDeviceContentController.h"
#import "LCBrowserIncidentsContentController.h"
#import "LCBrowserCasesContentController.h"
#import "LCBrowserTreeIncidents.h"
#import "LCBetaRegistrationWindowController.h"
#import "UKCrashReporter.h"
#import "LCFeedbackController.h"
#import "LCBrowserTreeCases.h"
#import "LCBrowserBonjourContentController.h"
#import "LCBrowserTreeBonjour.h"
#import "LCBrowserNetScanContentController.h"
#import "LCBrowserTreeNetworkScan.h"
#import "LCDocumentEditWindowController.h"
#import "LCBrowserSceneContentController.h" 
#import "LCBrowserVRackContentController.h" 
#import "LCSSceneDocument.h"
#import "LCBrowserSceneCollectionContentController.h"
#import "LCBrowserVRackCollectionContentController.h"
#import "LCBrowserTreeScenes.h"
#import "LCBrowserTreeVRacks.h"
#import "LCBrowserSiteContentController.h"
#import "LCBrowserCustomerContentController.h"
#import "LCGroupEditWindowController.h"
#import "LCBrowserTreeGroupsCustomer.h"
#import "LCBrowserServicesContentController.h"
#import "LCBrowserProcessesContentController.h"
#import "LCBrowserTreeServices.h"
#import "LCBrowserTreeProcesses.h"
#import "LCBrowserGettingStartedContentController.h"
#import "LCBrowserGroupContentController.h"
#import "LCBrowserTreeCoreProperty.h"
#import "LCBrowserTreeCoreDeployment.h"
#import "LCProcessProfileEditWindowController.h"

static NSMutableArray *activeControllers = nil;

@interface LCBrowser2Controller (private)
- (void) resizeStatusBarContentView;
@end

@implementation LCBrowser2Controller

#pragma mark "Class Methods"

+ (NSMutableArray *) activeControllers
{ return activeControllers; }

#pragma mark "Initialisation"

- (LCBrowser2Controller *) init
{
	/* Set sort descriptors */
	[self setCustSortDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"properties.desc" ascending:YES] autorelease], nil]];	
	[self setSiteSortDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"properties.suburb" ascending:YES] autorelease], nil]];	
	[self setDeviceSortDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"properties.name" ascending:YES] autorelease], nil]];	
	[self setContainerSortDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"properties.prio" ascending:NO] autorelease], nil]];	
	[self setObjectSortDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"indexNumber" ascending:YES] autorelease], nil]];	
	
	/* Set Filters */
	entityPresenceFilter = [[NSPredicate predicateWithFormat:@"presenceConfirmed == YES"] retain];
	customerEnabledFilter = [[NSPredicate predicateWithFormat:@"disabled == NO"] retain];
	
	/* Create browser treee */
	[self setBrowserTree:[LCBrowserTree browserTree]];

	/* Inform console controller */
	[[LCConsoleController masterController] newBrowserDidOpen:self];
	
	/* Load NIB */
	[super initWithWindowNibName:@"Browser2"];
	[self window];
	[[self window] setMovableByWindowBackground:NO];
	[contentTabView selectTabViewItemWithIdentifier:@"content"];
	[browserTree setTreeController:browserTreeController];
	[browserTree setOutlineView:browserTreeOutlineView];
	[self setShouldCascadeWindows:NO];
	[[[self window] contentView] setImage:nil];
	
	/* Rebuild Browser Tree */
	[browserTree resetRootItems];
	
	/* Window setup */
	[contentTabView setDrawsBackground:NO];
	[[barAnalyseButton cell] setHighlightsBy:0];
	[[barRefreshButton cell] setHighlightsBy:0];
	[[barGraphButton cell] setHighlightsBy:0];
	if ([[NSUserDefaults standardUserDefaults] objectForKey:@"LCBrowser2SplitViewPosition"])
	{ [treeSplitView setPosition:[[NSUserDefaults standardUserDefaults] floatForKey:@"LCBrowser2SplitViewPosition"] ofDividerAtIndex:0]; }
	[self setContentViewController:nil];
	
	/* KVO */
	[browserTreeController addObserver:self
							forKeyPath:@"selection" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							   context:NULL];	
	
	/* Display */
	[[self window] makeKeyAndOrderFront:self];
	[[LCConsoleController masterController] setBrowserOpened:YES];
	
	/* Add to active controller */
	if (!activeControllers)
	{ activeControllers = [[NSMutableArray array] retain]; }
	[activeControllers addObject:self];
	
	/* Check for Crashes */
	UKCrashReporterCheckForCrash (self);
	
	/* Display getting started */
	self.contentViewController = nil;
	
	
	return self;
}

- (LCBrowser2Controller *) initWithEntity:(LCEntity *)entity
{
	/* Creates a new browser that browses to the 
	* specified entity 
	*/
	
	/* Initialise */
	[self init];
	
	/* Set selection */
	[self selectEntity:entity];
	
	return self;
}

- (void) dealloc 
{
	[browserTree release];
	[custSortDescriptors release];
	[siteSortDescriptors release];
	[deviceSortDescriptors release];
	[containerSortDescriptors release];
	[objectSortDescriptors release];
	
	[super dealloc];
}

#pragma mark "Encoding"

//- (void)encodeWithCoder:(NSCoder *)encoder
//{
//	NSArray *selectedEntities = [self selectedEntities:self];
//	if ([selectedEntities count] > 0)
//	{ [encoder encodeObject:[selectedEntities objectAtIndex:0]  forKey:@"selectedEntity"]; }
//	[encoder encodeObject:NSStringFromRect([[self window] frame]) forKey:@"frame"];
//	[encoder encodeInt:[self deviceViewMode] forKey:@"deviceViewMode"];
//}
//
//- (LCBrowser2Controller *) initWithCoder:(NSCoder *)decoder
//{
//	LCEntity *selectedEntity = [decoder decodeObjectForKey:@"selectedEntity"];
// 	coderSelectedEntity = [selectedEntity retain];
//	if ([decoder decodeObjectForKey:@"frame"])
//	{
//		if (deviceSpecificViewAnimation)
//		{ [deviceSpecificViewAnimation stopAnimation]; }
//		[[self window] setFrame:NSRectFromString([decoder decodeObjectForKey:@"frame"]) display:YES]; 
//	}
//	[self setDeviceViewMode:[decoder decodeIntForKey:@"deviceViewMode"]];
//	[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(initWithCoderTimerCallback) userInfo:nil repeats:NO];
//    return self;
//}
//
//- (void) initWithCoderTimerCallback
//{
//	[self initWithEntity:coderSelectedEntity];	
//	[coderSelectedEntity release];
//}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"BrowserToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: YES];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
		
	/* Add Device/Service/Etc */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Add New..."] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (entityTreeAddButtonClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"add_drop_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Refresh */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Refresh"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshEntityClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Incident Manager */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Incident Manager"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (incidentManagerClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"event_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Open Case */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Open Case"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (openCaseForSelectedClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseOpen.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Case Search */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Case Search"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (caseSearchClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"casesearch_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Setup */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Lithium Setup"] autorelease];
	[tbitem setLabel:@"Lithium Setup"];
	[tbitem setImage:[NSImage imageNamed:@"configure_32.tif"]];
	[tbitem setToolTip:@"Lithium Core Setup, Action Script, Service Scripts, etc"];
	[tbitem setAction: @selector (lithiumSetupClicked:)];
	[toolbarItems setObject:tbitem forKey:@"Lithium Setup"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* New case for selected entities */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Manage Actions"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (actionListClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"tools_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Graph entities */
    tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Graph Selected"] autorelease];
    [tbitem setLabel:@"Graph Selected"];
	[tbitem setTarget:self];
	[tbitem setAction:@selector(graphSelectedClicked:)];
    [tbitem setImage:[NSImage imageNamed:@"Chart.tiff"]];
    [tbitem setToolTip:@"Select an Object or Metric and click here to graph it"];
    [toolbarItems setObject:tbitem forKey:@"Graph Selected"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
		
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/*
	 * Non-Defaults 
	 */

    /* Device View */
    tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Toggle Device View"] autorelease];
    [tbitem setLabel:@"Hide Device Visual"];
    [tbitem setTarget: self];
	[tbitem setAction:@selector(toggleDeviceViewClicked:)];
    [tbitem setImage:[NSImage imageNamed:@"ZoomToFit.tiff"]];
    [toolbarItems setObject:tbitem forKey:@"Toggle Device View"];
	toggleDeviceViewItem = tbitem;
	
    /* Metric History */
    tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Metric History"] autorelease];
    [tbitem setLabel:@"Metric History"];
	[tbitem setTarget:self];
	[tbitem setAction:@selector(metricHistoryClicked:)];
    [tbitem setImage:[NSImage imageNamed:@"MetricHistory.tiff"]];
    [tbitem setToolTip:@"Select a Metric in the Tree View and click here to perform trend analysis"];
    [toolbarItems setObject:tbitem forKey:@"Metric History"];
	
    /* Analyse */
    tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Analyse Selected"] autorelease];
    [tbitem setLabel:@"Analyse Selected"];
	[tbitem setTarget:self];
	[tbitem setAction:@selector(analyseSelectedClicked:)];
    [tbitem setImage:[NSImage imageNamed:@"TrendAnalysis.tiff"]];
    [tbitem setToolTip:@"Select an Object in Object View or Tree View and click here to perform trend analysis"];
    [toolbarItems setObject:tbitem forKey:@"Analyse Selected"];
	
	[[self window] setToolbar:toolbar];
}

- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag 
{ return [toolbarItems objectForKey: identifier]; }

- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar 
{ return [toolbarItems allKeys]; }

- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarDefaultItems; }

- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar 
{ return toolbarSelectableItems; }

- (IBAction) toggleToolbarClicked:(id)sender
{ [[self window] toggleToolbarShown:sender]; }

- (IBAction) dummyClicked:(id)sender
{
	
}

#pragma mark "UI Item Validation"

- (BOOL) validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	/*
	 * Toolbar
	 */
	
	if (action == @selector(lithiumSetupClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(entityTreeAddButtonClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(incidentManagerClicked:))
	{ return YES; }

	if (action == @selector(actionListClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(manageActionScriptsClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	
	/*
	 * Lithium
	 */
	
	if (action == @selector(addNewSiteClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(editSiteClicked:))
	{ if ([self.selectedEntity site] == self.selectedEntity && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(removeSiteClicked:))
	{ if ([self.selectedEntity site] == self.selectedEntity && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(addNewDeviceClicked:) && [[self.selectedEntity customer] userIsAdmin])
	{ if ([self.selectedEntity site]) return YES; }

	if (action == @selector(setupAssistantClicked:) && [[self.selectedEntity customer] userIsAdmin])
	{ if ([self.selectedEntity customer]) return YES; }
	
	if (action == @selector(addNewActionClicked:) && [[self.selectedEntity customer] userIsAdmin])
	{ if ([self.selectedEntity customer]) return YES; }	

	if (action == @selector(addNewEmailActionClicked:) && [[self.selectedEntity customer] userIsAdmin])
	{ if ([self.selectedEntity customer]) return YES; }	
	
	/* Lithium -> Case */
	
	if (action == @selector(newCaseClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsNormal]) return YES; }

	if (action == @selector(reOpenCaseClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsNormal]) return YES; }
	
	if (action == @selector(caseSearchClicked:))
	{ if ([self.selectedEntity customer]) return YES; }
	
	/*
	 * Device
	 */

	if (action == @selector(duplicateDeviceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(editDeviceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(removeDeviceClicked:))
	{ if ([self.selectedEntity device] == self.selectedEntity && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(reviewActiveTriggersClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(restartMonitoringProcessClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(resetDeviceTriggerRulesClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(markDeviceInProductionClicked:) ||
		action == @selector(markDeviceOutOfServiceClicked:) ||
		action == @selector(markDeviceInTestingClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	/* Device -> Monitored Service */
	
	if (action == @selector(addNewServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }

	if (action == @selector(addNewHTTPServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	if (action == @selector(addNewPOPServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	if (action == @selector(addNewSMTPServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	if (action == @selector(addNewIMAPServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	if (action == @selector(addNewDNSServiceClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(editServiceClicked:))
	{ 
		if ([self.selectedEntity object] && [[[self.selectedEntity container] name] isEqualToString:@"service"] && [[self.selectedEntity customer] userIsAdmin])
		{ return YES; }
	}	
	
	if (action == @selector(removeServiceClicked:))
	{ 
		if ([self.selectedEntity object] && [[[self.selectedEntity container] name] isEqualToString:@"service"] && [[self.selectedEntity customer] userIsAdmin])
		{ return YES; }
	}
	
	if (action == @selector(manageServiceScriptsClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	/* Device -> Add Process Profile */
	
	if (action == @selector(addNewProcessProfileClicked:))
	{ if ([self.selectedEntity device] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	/* Device -> Connect to Device Using */
	
	if ([NSStringFromSelector(action) hasPrefix:@"connectUsing"])
	{ if ([self.selectedEntity device]) return YES; }
	
	/* 
	 * Monitored Entity
	 */
	
	if (action == @selector(refreshSelectedClicked:))
	{ if (self.selectedEntity) return YES; }
	
	if (action == @selector(faultHistoryClicked:)) 
	{ if ([self.selectedEntity customer]) return YES; }

	if (action == @selector(browseToSelectedClicked:))
	{ if ([self.selectedEntity customer]) return YES; }

	if (action == @selector(refreshEntityClicked:))
	{ if ([self.selectedEntity customer]) return YES; }

	if (action == @selector(openCaseForSelectedClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsNormal]) return YES; }

	if (action == @selector(resetTriggerRulesClicked:))
	{ if ([self.selectedEntity customer] && [[self.selectedEntity customer] userIsAdmin]) return YES; }
	
	if (action == @selector(copyWebUrlToClipBoardClicked:))
	{ if ([self.selectedEntity customer]) return YES; }

	if (action == @selector(openWebUrlInBrowserClicked:))
	{ if ([self.selectedEntity customer]) return YES; }
	
	/*
	 * Add Menu
	 */
	
	if (action == @selector(addNewDeviceToSiteMenuCicked:))
	{ return YES; }
	if (action == @selector(importARDToSiteMenuCicked:))
	{ return YES; }
	if (action == @selector(importCSVToSiteMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewSiteToCustomerMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewServiceToDeviceMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewEmailActionToCustomerMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewActionToCustomerMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewSceneToCustomerMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewVRackToCustomerMenuCicked:))
	{ return YES; }
	if (action == @selector(addNewGroupToCustomerMenuCicked:))
	{ return YES; }
	
	/* 
	 * Documents 
	 */
	
	if (action == @selector(deleteSelectedDocumentClicked:))
	{ if ([[treeSelectedObject class] isSubclassOfClass:[LCDocument class]]) return YES; }	
	if (action == @selector(renameSelectedDocumentClicked:))
	{ if ([[treeSelectedObject class] isSubclassOfClass:[LCDocument class]]) return YES; }
	if (action == @selector(refreshDocumentListClicked:))
	{ return YES; }
	
	/*
	 * Groups 
	 */
	
	if (action == @selector(addNewGroupToSelectedClicked:))
	{ return YES; }
	if (action == @selector(refreshGroupsClicked:))
	{ return YES; }
	if (action == @selector(deleteSelectedGroupClicked:))
	{ if ([[treeSelectedObject class] isSubclassOfClass:[LCGroup class]]) return YES; }
	if (action == @selector(removeSelectedEntityFromGroupClicked:))
	{ if ([[treeSelectedObject class] isSubclassOfClass:[LCEntity class]]) return YES; }
	if (action == @selector(renameSelectedGroupClicked:))
	{ if ([[treeSelectedObject class] isSubclassOfClass:[LCGroup class]]) return YES; }
	
	/*
	 * Faults 
	 */
	
	if (action == @selector(refreshIncidentsClicked:)) return YES;
	if (action == @selector(refreshCasesClicked:)) return YES;

	/*
	 * Applications 
	 */
	
	if (action == @selector(refreshServicesClicked:)) return YES;
	if (action == @selector(refreshProcessesClicked:)) return YES;
	
	/*
	 * Deployments
	 */
	
	if (action == @selector(refreshDeploymentClicked:)) return YES;
	
	
	return NO;
}

#pragma mark "Window Delegate Methods"

- (BOOL)windowShouldClose:(id)window
{
	if (self.contentViewController && ![self.contentViewController treeSelectionCanChangeToRepresent:nil])
	{
		return NO;
	}
	else
	{ 
		return YES;
	}
}

- (void) windowWillClose:(NSNotification *)notification
{
	[activeControllers removeObject:self];
	self.contentViewController = nil;
	
	/* Remove KVO */
	[browserTreeController removeObserver:self forKeyPath:@"selection"];	
	
	/* Notify ConsoleController */
	[[LCConsoleController masterController] browserDidClose:self];
	
	/* Clear controller */
	[controllerAlias setContent:nil];
}

- (void) windowDidBecomeMain:(NSNotification *)aNotification
{
	[[LCConsoleController masterController] browserDidBecomeMainWindow:self];
}

- (void) windowDidResignMain:(NSNotification *)aNotification
{

}

- (void) windowFirstResponderDidChange:(NSResponder *)responder
{
	
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
	/* Bump posotion in array */
	[activeControllers removeObject:self];
	[activeControllers addObject:self];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if (object == browserTreeController)
	{
		if ([[browserTreeController selectedObjects] count] > 0)
		{
			/* Object is selected */
			
			/* Check if we're in a search and cancel it */
			if (self.searchString && [[browserTreeController selectedObjects] objectAtIndex:0] != self.treeSelectedObject)
			{ self.searchString = nil; }
			
			/* Set tree selection */
			self.treeSelectedObject = [[browserTreeController selectedObjects] objectAtIndex:0];
		}
		else
		{
			/* No Selection */
			self.treeSelectedObject = nil;			
		}
	}
}

#pragma mark "Tree Selection"

@synthesize treeSelectedObject;
- (void) setTreeSelectedObject:(id)value
{
	/* Check to make sure there is a real change */
	if (treeSelectedObject == value)
	{ return; }
	
	/* Set value */
	treeSelectedObject = value;
	
	/* Set Entity Selection */
	if ([[treeSelectedObject class] isSubclassOfClass:[LCEntity class]])
	{
		self.treeSelectedEntity = (LCEntity *) treeSelectedObject;
	}
	else
	{
		self.treeSelectedEntity = nil;
	}
	
	/* Set Content View etc based on selection */
	if ([treeSelectedObject class] == [LCCustomer class])
	{
		/* Customer Selected */
		self.contentViewController = [[LCBrowserCustomerContentController alloc] initWithCustomer:treeSelectedObject inBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCSite class])
	{
		/* Site Selected */
		self.contentViewController = [[LCBrowserSiteContentController alloc] initWithSite:treeSelectedObject inBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCDevice class])
	{
		/* Device Selected */
		self.contentViewController = [[LCBrowserDeviceContentController alloc] initWithDevice:treeSelectedObject inBrowser:self];
	}
	else if (treeSelectedEntity && treeSelectedEntity.type > 3)
	{
		self.contentViewController = [[LCBrowserDeviceContentController alloc] initWithDevice:treeSelectedEntity.device inBrowser:self];
		[self.contentViewController selectEntity:treeSelectedEntity];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeIncidents class])
	{
		/* Incidents */
		self.contentViewController = [[LCBrowserIncidentsContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeCases class])
	{
		/* Cases */
		self.contentViewController = [[LCBrowserCasesContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeBonjour class])
	{
		/* Discovery - Bonjour */
		self.contentViewController = [[LCBrowserBonjourContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeNetworkScan class])
	{
		/* Discovery - Network Scan */
		self.contentViewController = [[LCBrowserNetScanContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeScenes class])
	{
		/* Documents - Scene Collection */
		self.contentViewController = [[LCBrowserSceneCollectionContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCSSceneDocument class])
	{
		/* Documents - Scene */
		LCSSceneDocument *document = (LCSSceneDocument *) treeSelectedObject;
		self.contentViewController = [[LCBrowserSceneContentController alloc] initWithDocument:[document copyDocument] inBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeVRacks class])
	{
		/* Documents - Rack Collection */
		self.contentViewController = [[LCBrowserVRackCollectionContentController alloc] initInBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCVRackDocument class])
	{
		/* Documents - Rack */
		LCVRackDocument *document = (LCVRackDocument *) treeSelectedObject;
		self.contentViewController = [[LCBrowserVRackContentController alloc] initWithDocument:[document copyDocument] inBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeServices class])
	{
		/* Applications - Services */
		self.contentViewController = [[LCBrowserServicesContentController alloc] initWithBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeProcesses class])
	{
		/* Applications - Processes */
		self.contentViewController = [[LCBrowserProcessesContentController alloc] initWithBrowser:self];
	}
	else if ([treeSelectedObject class] == [LCGroup class])
	{
		/* Groups */
		self.contentViewController = [[LCBrowserGroupContentController alloc] initWithGroup:treeSelectedObject inBrowser:self];
	}
	else if ([[treeSelectedObject class] isSubclassOfClass:[LCBrowserTreeCoreProperty class]])
	{
		/* Core Property */
		LCBrowserTreeCoreProperty *property = (LCBrowserTreeCoreProperty *) treeSelectedObject;
		self.contentViewController = [[property.contentControllerClass alloc] initWithCustomer:property.customer inBrowser:self];
	}
	else
	{
		/* No/Unknown Selection */
		self.contentViewController = nil;
	}
}

@synthesize treeSelectedEntity;

#pragma mark "Global Selection Methods (Not KVO Observable)"

- (LCEntity *) selectedEntity
{
	if (self.treeSelectedEntity)
	{
		return self.treeSelectedEntity;
	}
	else if ([[LCCustomerList masterArray] count] == 1)
	{
		/* If there's only one site under one customer, then use
		 * that as the default selection.
		 * Otherwise, if there's only one customer but multiple sites
		 * use the customer as the default selection
		 */
		LCCustomer *defaultCustomer = [[LCCustomerList masterArray] objectAtIndex:0];
		if ([[defaultCustomer children] count] == 1)
		{ return [defaultCustomer.children objectAtIndex:0]; }
		else
		{ return (LCEntity *) defaultCustomer; }
	}
	else
	{ return nil; }
}

- (NSArray *) selectedEntities
{
	if (self.treeSelectedEntity)
	{ return [NSArray arrayWithObject:self.treeSelectedEntity]; }
	else
	{ return [NSArray array]; }
} 

#pragma mark "Selection Management"

- (void) selectEntity:(LCEntity *)entity
{
	/* Selects the entity in the tree view 
	 * And then calls selectEntity on the 
	 * current contentView controller
	 */
	
	[browserTreeOutlineView selectEntity:entity];
	[contentViewController selectEntity:entity];
}

- (void) selectIncident:(LCIncident *)incident
{
	[browserTreeOutlineView selectIncidentsItem];
	[contentViewController selectIncident:incident];
}

#pragma mark "Content Pane Methods"

- (void) resizeStatusBarContentView
{
	float inspectorOffset;
	inspectorOffset = (NSMaxX([[statusBarContentView superview] bounds]) - NSMinX([searchField frame])) + 8.0;
	if (contentViewController.inspectorView && contentViewController.preferredInspectorWidth > inspectorOffset)
	{
		inspectorOffset = contentViewController.preferredInspectorWidth;
	}
	
	statusBarContentView.frame = NSMakeRect(NSMinX([[statusBarContentView superview] bounds]) + NSWidth([treeSplitViewTreeView frame]),
											NSMinY([statusBarContentView frame]),
											NSWidth([[statusBarContentView superview] bounds]) - NSWidth([treeSplitViewTreeView frame]) - inspectorOffset,
											NSHeight([treeSplitViewTreeView bounds]));	
}

@synthesize contentViewController;
- (void) setContentViewController:(LCBrowser2ContentViewController *)value
{
	/* Remove old content */
	[[contentViewController view] removeFromSuperview];
	[[contentViewController bottomView] removeFromSuperview];
	[[contentViewController inspectorView] removeFromSuperview];
	[[contentViewController statusBarView] removeFromSuperview];
	[contentViewController removedFromBrowserWindow];
	[contentViewController release];
	
	/* Remove enclosing views */
	[topContentView removeFromSuperview];
	[bottomContentEnclosingView removeFromSuperview];
	[inspectorContentView removeFromSuperview];
	
	/* View Rects */
	NSRect contentRect = [contentEnclosingView bounds];
	
	/* Add New */
	contentViewController = [value retain];
	if (contentViewController)
	{
		/*
		 * First setup the container views, do not add the
		 * actual content views from the controller yet
		 */
		
		/* Add top enclosing view to content enclosing view*/
		
		/* Initially, size the top content view to the full content size */
		[topContentView setFrame:[contentEnclosingView bounds]];
		[topContentView setAutoresizingMask:NSViewHeightSizable|NSViewMinYMargin|NSViewMaxXMargin|
											NSViewWidthSizable|NSViewMinXMargin|NSViewMinXMargin];

		
		/* Check for bottom view */
		if (contentViewController.bottomView)
		{
			/* Bottom view is present, handling will depend on which
			 * component (top or bottom) is to be fixed
			 */
			if (contentViewController.resizeMode == RESIZE_TOP)
			{
				/* Bottom is fixed and uses the preferredFixedComponentHeight 
				 * Top takes all remaining space
				 * Resizing mask is set to allow top view to resize height and width
				 *
				 * Only the bottomContentEnclosingView is sized and the +48 accounts 
				 * for the mid bar. 
				 */
				NSRect bottomRect = NSMakeRect(NSMinX(contentRect), NSMinY(contentRect), 
											   NSWidth(contentRect), [contentViewController preferredFixedComponentHeight] + 48);
				NSRect topRect = NSMakeRect(NSMinX(contentRect), NSMaxY(bottomRect),
											NSWidth(contentRect), NSHeight(contentRect) - NSHeight(bottomRect));
				[bottomContentEnclosingView setFrame:bottomRect];
				[topContentView setFrame:topRect];
				[topContentView setAutoresizingMask:NSViewHeightSizable|NSViewMaxYMargin|NSViewWidthSizable];
				[bottomContentEnclosingView setAutoresizingMask:NSViewWidthSizable];
			}
			else if (contentViewController.resizeMode == RESIZE_BOTTOM)
			{
				/* Top is fixed and uses the preferredFixedComponentHeight 
				 * Bottom takes all remaining space
				 * Resizing mask is set to allow bottom view to resize height and width
				 */
				NSRect topRect = NSMakeRect(NSMinX(contentRect), NSMaxY(contentRect) - [contentViewController preferredFixedComponentHeight],
											NSWidth(contentRect), [contentViewController preferredFixedComponentHeight]);
				NSRect bottomRect = NSMakeRect(NSMinX(contentRect), NSMinY(contentRect),
											   NSWidth(contentRect), NSHeight(contentRect) - NSHeight(topRect));
				[bottomContentEnclosingView setFrame:bottomRect];
				[topContentView setFrame:topRect];
				[topContentView setAutoresizingMask:NSViewMinYMargin|NSViewWidthSizable];
				[bottomContentEnclosingView setAutoresizingMask:NSViewHeightSizable|NSViewMinYMargin|NSViewWidthSizable];
			}
			else if (contentViewController.resizeMode == RESIZE_BOTH)
			{
				/* Both top and bottom resize
				 * Sizing is done 50/50 split between top and bottom
				 * Resizing mask is set to allow both top and bottom views to resize
				 */
				NSRect bottomRect = NSMakeRect(NSMinX(contentRect), NSMinY(contentRect),
											   NSWidth(contentRect), NSHeight(contentRect)*0.5);
				NSRect topRect = NSMakeRect(NSMinX(contentRect), NSMaxY(bottomRect),
											NSWidth(contentRect), NSHeight(contentRect)*0.5);
				[bottomContentEnclosingView setFrame:bottomRect];
				[topContentView setFrame:topRect];
				[topContentView setAutoresizingMask:NSViewHeightSizable|NSViewMinYMargin|NSViewMaxYMargin|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin];
				[bottomContentEnclosingView setAutoresizingMask:NSViewHeightSizable|NSViewMinYMargin|NSViewMaxYMargin|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin];
			}			
		}
		
		/* Check for inspector */
		if (contentViewController.inspectorView)
		{
			/* Inspector is present, size it using the preferredInspectorWidth */
			NSRect inspectorRect = NSMakeRect(NSMaxX(contentRect) - contentViewController.preferredInspectorWidth, NSMinY(contentRect),
											  contentViewController.preferredInspectorWidth, NSHeight(contentRect));
			[inspectorContentView setFrame:inspectorRect];
			
			/* Resize top and bottom enclosing views to accomodate inspector */
			NSRect topRect = [topContentView frame];
			topRect.size.width = topRect.size.width - contentViewController.preferredInspectorWidth;
			[topContentView setFrame:topRect];
			if (contentViewController.bottomView)
			{
				NSRect bottomRect = [bottomContentEnclosingView frame];
				bottomRect.size.width = bottomRect.size.width - contentViewController.preferredInspectorWidth;
				[bottomContentEnclosingView setFrame:bottomRect];
			}
			
			/* Resize search field to match inspector width */
			float searchWidth = contentViewController.preferredInspectorWidth - 2.0;
			NSView *searchEnclosingView = [searchField superview];
			[searchField setFrame:NSMakeRect(NSMaxX([searchEnclosingView bounds]) - searchWidth,
											 NSMinY([searchEnclosingView bounds]) + 6.0, 
											 searchWidth - 20.0, 22.0)];
			
		}					
		else
		{
			/* Reset position of search field */
			float defaultSearchWidth = 188.0;
			NSView *searchEnclosingView = [searchField superview];
			[searchField setFrame:NSMakeRect(NSMaxX([searchEnclosingView bounds]) - defaultSearchWidth - 20.0,
											 NSMinY([searchEnclosingView bounds]) + 6.0, 
											 defaultSearchWidth, 22.0)];
		}

			
		/* Check for status bar */
		if (contentViewController.statusBarView)
		{
			/* This is a simpler setup, there is either a statusBar View
			 * present or not. No fancy resizing needed, just size it
			 * setup the responder chain and add it 
			 */
			[self resizeStatusBarContentView];
		}
		
		/* Add content enclosing views to root content view 
		 * These are still the containers, not the actual content
		 * views from the controller
		 */
		
		[contentEnclosingView addSubview:topContentView];
		if (contentViewController.bottomView) [contentEnclosingView addSubview:bottomContentEnclosingView];
		if (contentViewController.inspectorView) [contentEnclosingView addSubview:inspectorContentView];
		
		/*
		 * Now size and add the live content views from the content controller
		 */
		
		/* Top view */
		[contentViewController.view setFrame:[topContentView bounds]];
		[topContentView addSubview:contentViewController.view];
		[contentViewController setNextResponder:[self window]];
		[contentViewController.view setNextResponder:contentViewController];
		
		/* Bottom view */
		if (contentViewController.bottomView)
		{
			[contentViewController.bottomView setFrame:[bottomContentView bounds]];
			[bottomContentView addSubview:contentViewController.bottomView];
			[contentViewController.bottomView setNextResponder:contentViewController];
		}
		
		/* Inspector */
		if (contentViewController.inspectorView)
		{
			[contentViewController.inspectorView setFrame:[inspectorContentView bounds]];
			[inspectorContentView addSubview:contentViewController.inspectorView];
			[contentViewController.inspectorView setNextResponder:contentViewController];
		}		
		
		/* Status Bar */
		if (contentViewController.statusBarView)
		{
			contentViewController.statusBarView.frame = statusBarContentView.bounds;
			[statusBarContentView addSubview:contentViewController.statusBarView];
			[contentViewController.statusBarView setNextResponder:contentViewController];
		}
	}
	else
	{
		/* No content view controller is present, add the top enclosing view
		 * blank with no subviews, fully sized and resizeable as a placeholder 
		 */
		contentViewController = nil;
		[topContentView setFrame:[contentEnclosingView bounds]];
		[topContentView setAutoresizingMask:NSViewHeightSizable|NSViewMinYMargin|NSViewMaxYMargin|NSViewWidthSizable|NSViewMinXMargin|NSViewMaxXMargin];		
		[contentEnclosingView addSubview:topContentView];
	}	
}


#pragma mark "SplitView Delegate Methods"

- (void)splitViewDidResizeSubviews:(NSNotification *)aNotification
{
	[self resizeStatusBarContentView];
	[browserTreeOutlineView setNeedsDisplay];
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainSplitPosition:(CGFloat)proposedPosition ofSubviewAt:(NSInteger)dividerIndex
{
	[[NSUserDefaults standardUserDefaults] setFloat:proposedPosition forKey:@"LCBrowser2SplitViewPosition"];
	[[NSUserDefaults standardUserDefaults] synchronize];
	if (proposedPosition > 173.0) 
	{
		if ((NSWidth([splitView frame]) - proposedPosition) < 870.0)
		{
			/* Not leaving enough space for content */
			return NSWidth([splitView frame]) - 870.0;
		}
		else
		{ return proposedPosition; }
	}
	else 
	{
		/* Not leaving enough space for tree */
		return 173.0;
	}
}

- (BOOL)splitView:(NSSplitView *)splitView canCollapseSubview:(NSView *)subview
{
	return YES;
}

- (BOOL)splitView:(NSSplitView *)splitView shouldCollapseSubview:(NSView *)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex
{
	[treeSplitView setPosition:241.0 ofDividerAtIndex:0];
	return NO;
}

#pragma mark "Add Menu Methods"

- (IBAction) entityTreeAddButtonClicked:(id)sender
{
	/* Clear old */
	while ([[[addDeviceMenuItem submenu] itemArray] count] > 0) 
	{ [[addDeviceMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addImportCSVMenuItem submenu] itemArray] count] > 0) 
	{ [[addImportCSVMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addImportARDMenuItem submenu] itemArray] count] > 0) 
	{ [[addImportARDMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addDeviceMenuItem submenu] itemArray] count] > 0) 
	{ [[addDeviceMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addSiteMenuItem submenu] itemArray] count] > 0) 
	{ [[addSiteMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addServiceMenuItem submenu] itemArray] count] > 0) 
	{ [[addServiceMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addActionMenuItem submenu] itemArray] count] > 0) 
	{ [[addActionMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addSceneMenuItem submenu] itemArray] count] > 0) 
	{ [[addSceneMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addVRackMenuItem submenu] itemArray] count] > 0) 
	{ [[addVRackMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addSceneMenuItem submenu] itemArray] count] > 0) 
	{ [[addSceneMenuItem submenu] removeItemAtIndex:0]; }
	while ([[[addGroupMenuItem submenu] itemArray] count] > 0) 
	{ [[addGroupMenuItem submenu] removeItemAtIndex:0]; }
	
	/* Construct Device Menu */
	NSMenuItem *item;
	if ([self.selectedEntity site])
	{
		/* Add selected site at top with a separator */
		item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [[self.selectedEntity site] displayString], [[self.selectedEntity customer] displayString]]
										  action:@selector(addNewDeviceToSiteMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:[self.selectedEntity site]];
		[[addDeviceMenuItem submenu] insertItem:item atIndex:[[[addDeviceMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	NSMutableArray *sites = [NSMutableArray array];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ [sites addObjectsFromArray:customer.children]; }
	if ([sites count] > 1)
	{ 
		if ([[[addDeviceMenuItem submenu] itemArray] count] > 0)
		{ [[addDeviceMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addDeviceMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each site */
			for (LCSite *site in customer.children)
			{
				item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [site displayString], [site.customer displayString]]
												  action:@selector(addNewDeviceToSiteMenuCicked:) 
										   keyEquivalent:@""];
				[item setTarget:self];
				[item setRepresentedObject:site];
				[[addDeviceMenuItem submenu] insertItem:item atIndex:[[[addDeviceMenuItem submenu] itemArray] count]];			
				[item autorelease];
			}
		}
	}

	/* Construct Import from CSV Menu */
	if ([self.selectedEntity site])
	{
		/* Add selected site at top with a separator */
		item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [[self.selectedEntity site] displayString], [[self.selectedEntity customer] displayString]]
										  action:@selector(importCSVToSiteMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:[self.selectedEntity site]];
		[[addImportCSVMenuItem submenu] insertItem:item atIndex:[[[addImportCSVMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	sites = [NSMutableArray array];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ [sites addObjectsFromArray:customer.children]; }
	if ([sites count] > 1)
	{ 
		if ([[[addImportCSVMenuItem submenu] itemArray] count] > 0)
		{ [[addImportCSVMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addImportCSVMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each site */
			for (LCSite *site in customer.children)
			{
				item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [site displayString], [site.customer displayString]]
												  action:@selector(importCSVToSiteMenuCicked:) 
										   keyEquivalent:@""];
				[item setTarget:self];
				[item setRepresentedObject:site];
				[[addImportCSVMenuItem submenu] insertItem:item atIndex:[[[addImportCSVMenuItem submenu] itemArray] count]];			
				[item autorelease];
			}
		}
	}	

	/* Construct Import from ARD Menu */
	if ([self.selectedEntity site])
	{
		/* Add selected site at top with a separator */
		item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [[self.selectedEntity site] displayString], [[self.selectedEntity customer] displayString]]
										  action:@selector(importARDToSiteMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:[self.selectedEntity site]];
		[[addImportARDMenuItem submenu] insertItem:item atIndex:[[[addImportARDMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	[NSMutableArray array];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ [sites addObjectsFromArray:customer.children]; }
	if ([sites count] > 1)
	{ 
		if ([[[addImportARDMenuItem submenu] itemArray] count] > 0)
		{ [[addImportARDMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addImportARDMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each site */
			for (LCSite *site in customer.children)
			{
				item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ (%@)", [site displayString], [site.customer displayString]]
												  action:@selector(importARDToSiteMenuCicked:) 
										   keyEquivalent:@""];
				[item setTarget:self];
				[item setRepresentedObject:site];
				[[addImportARDMenuItem submenu] insertItem:item atIndex:[[[addImportARDMenuItem submenu] itemArray] count]];			
				[item autorelease];
			}
		}
	}		
	
	/* Construct Site Menu */
	if ([self.selectedEntity customer])
	{
		/* Add selected customer at top */
		item = [[NSMenuItem alloc] initWithTitle:[self.selectedEntity.customer displayString]
										  action:@selector(addNewSiteToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[addSiteMenuItem submenu] insertItem:item atIndex:[[[addSiteMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	if (![self.selectedEntity customer] || [[LCCustomerList masterArray] count] > 1)
	{ 
		if ([[[addSiteMenuItem submenu] itemArray] count] > 0)
		{ [[addSiteMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addSiteMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each customer */
			item = [[NSMenuItem alloc] initWithTitle:customer.displayString
											  action:@selector(addNewSiteToCustomerMenuCicked:) 
									   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[addSiteMenuItem submenu] insertItem:item atIndex:[[[addSiteMenuItem submenu] itemArray] count]];			
			[item autorelease];
		}
	}
	
	/* Construct Service Menu */
	if ([self.selectedEntity device])
	{
		/* Add selected device at top with a separator */
		LCDevice *device = self.selectedEntity.device;
		item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ @ %@ (%@)", device.displayString, [device.site displayString], [device.customer displayString]]
										  action:@selector(addNewServiceToDeviceMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:device];
		[[addServiceMenuItem submenu] insertItem:item atIndex:[[[addServiceMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	NSMutableArray *devices = [NSMutableArray array];
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ [devices addObjectsFromArray:[customer valueForKeyPath:@"children.@unionOfArrays.children"]]; }
	if (devices.count > 1)
	{ 
		if ([[[addServiceMenuItem submenu] itemArray] count] > 0)
		{ [[addServiceMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addServiceMenuItem submenu] itemArray] count]]; }
		for (LCDevice *device in devices)
		{
			/* Add item for each device */
			item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"%@ @ %@ (%@)", device.displayString, [device.site displayString], [device.customer displayString]]
											  action:@selector(addNewServiceToDeviceMenuCicked:) 
									   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:device];
			[[addServiceMenuItem submenu] insertItem:item atIndex:[[[addServiceMenuItem submenu] itemArray] count]];			
			[item autorelease];
		}
	}
	
	/* Construct Action Menu */
	if ([self.selectedEntity customer])
	{
		/* Add selected customer sub-menu at top */
		NSMenuItem *customerItem = [[NSMenuItem alloc] initWithTitle:[self.selectedEntity.customer displayString]
															  action:@selector(addNewSiteToCustomerMenuCicked:) 
													   keyEquivalent:@""];
		NSMenu *customerMenu = [[NSMenu alloc] init];
		[customerItem setSubmenu:customerMenu];
		[[addActionMenuItem submenu] insertItem:customerItem atIndex:[[[addActionMenuItem submenu] itemArray] count]];
		[customerItem autorelease];

		/* Email Alert Action */
		item = [[NSMenuItem alloc] initWithTitle:@"Email Alert Action"
										  action:@selector(addNewEmailActionToCustomerMenuCicked:) 
								   keyEquivalent:@""];		
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[customerItem submenu] insertItem:item atIndex:[[[customerItem submenu] itemArray] count]];
		[item autorelease];
		item = [[NSMenuItem alloc] initWithTitle:@"Other Action"
										  action:@selector(addNewActionToCustomerMenuCicked:) 
								   keyEquivalent:@""];		
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[customerItem submenu] insertItem:item atIndex:[[[customerItem submenu] itemArray] count]];
		[item autorelease];
	}
	if (![self.selectedEntity customer] || [[LCCustomerList masterArray] count] > 1)
	{ 
		if ([[[addActionMenuItem submenu] itemArray] count] > 0)
		{ [[addActionMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addActionMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add selected customer sub-menu at top */
			NSMenuItem *customerItem = [[NSMenuItem alloc] initWithTitle:[customer displayString]
																  action:@selector(addNewSiteToCustomerMenuCicked:) 
														   keyEquivalent:@""];
			NSMenu *customerMenu = [[NSMenu alloc] init];
			[customerItem setSubmenu:customerMenu];
			[[addActionMenuItem submenu] insertItem:customerItem atIndex:[[[addActionMenuItem submenu] itemArray] count]];
			[customerItem autorelease];
			
			/* Email Alert Action */
			item = [[NSMenuItem alloc] initWithTitle:@"Email Alert Action"
											  action:@selector(addNewEmailActionToCustomerMenuCicked:) 
									   keyEquivalent:@""];		
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[customerItem submenu] insertItem:item atIndex:[[[customerItem submenu] itemArray] count]];
			[item autorelease];
			item = [[NSMenuItem alloc] initWithTitle:@"Other Action"
											  action:@selector(addNewActionToCustomerMenuCicked:) 
									   keyEquivalent:@""];		
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[customerItem submenu] insertItem:item atIndex:[[[customerItem submenu] itemArray] count]];
			[item autorelease];
		}
	}
	
	/* Construct Scene Menu */
	if ([self.selectedEntity customer])
	{
		/* Add selected customer at top */
		item = [[NSMenuItem alloc] initWithTitle:[self.selectedEntity.customer displayString]
										  action:@selector(addNewSceneToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[addSceneMenuItem submenu] insertItem:item atIndex:[[[addSceneMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	if ([[LCCustomerList masterArray] count] > 1)
	{ 
		if ([[[addSceneMenuItem submenu] itemArray] count] > 0)
		{ [[addSceneMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addSceneMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each customer */
			item = [[NSMenuItem alloc] initWithTitle:customer.displayString
											  action:@selector(addNewSceneToCustomerMenuCicked:) 
									   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[addSceneMenuItem submenu] insertItem:item atIndex:[[[addSceneMenuItem submenu] itemArray] count]];			
			[item autorelease];
		}
	}

	/* Construct VRack Menu */
	if ([self.selectedEntity customer])
	{
		/* Add selected customer at top */
		item = [[NSMenuItem alloc] initWithTitle:[self.selectedEntity.customer displayString]
										  action:@selector(addNewVRackToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[addVRackMenuItem submenu] insertItem:item atIndex:[[[addVRackMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	if ([[LCCustomerList masterArray] count] > 1)
	{ 
		if ([[[addVRackMenuItem submenu] itemArray] count] > 0)
		{ [[addVRackMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addVRackMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each customer */
			item = [[NSMenuItem alloc] initWithTitle:customer.displayString
											  action:@selector(addNewVRackToCustomerMenuCicked:) 
									   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[addVRackMenuItem submenu] insertItem:item atIndex:[[[addVRackMenuItem submenu] itemArray] count]];			
			[item autorelease];
		}
	}

	/* Construct Group Menu */
	if ([self.selectedEntity customer])
	{
		/* Add selected customer at top */
		item = [[NSMenuItem alloc] initWithTitle:[self.selectedEntity.customer displayString]
										  action:@selector(addNewGroupToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:self.selectedEntity.customer];
		[[addGroupMenuItem submenu] insertItem:item atIndex:[[[addGroupMenuItem submenu] itemArray] count]];
		[item autorelease];
	}
	if ([[LCCustomerList masterArray] count] > 1)
	{ 
		if ([[[addGroupMenuItem submenu] itemArray] count] > 0)
		{ [[addGroupMenuItem submenu] insertItem:[NSMenuItem separatorItem] atIndex:[[[addGroupMenuItem submenu] itemArray] count]]; }
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each customer */
			item = [[NSMenuItem alloc] initWithTitle:customer.displayString
											  action:@selector(addNewGroupToCustomerMenuCicked:) 
									   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[[addGroupMenuItem submenu] insertItem:item atIndex:[[[addGroupMenuItem submenu] itemArray] count]];			
			[item autorelease];
		}
	}	
	
	/* Pop menu */
	[NSMenu popUpContextMenu:browserAddMenu withEvent:[NSApp currentEvent] forView:entityTreeAddButton];
}

- (IBAction) addNewDeviceToSiteMenuCicked:(NSMenuItem *)menuItem
{
	LCSite *site = [menuItem representedObject];
	if (site)
	{
		LCDeviceEditController *editController = [[LCDeviceEditController alloc] initForNewDeviceAtSite:site];
		[NSApp beginSheet:[editController window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

- (IBAction) importCSVToSiteMenuCicked:(NSMenuItem *)menuItem
{
	LCSite *site = [menuItem representedObject];
	if (site)
	{
		[[LCDeviceEditController alloc] initForImportFromCSVAtSite:site windowForSheet:[self window]];
	}
}

- (IBAction) importARDToSiteMenuCicked:(NSMenuItem *)menuItem
{
	LCSite *site = [menuItem representedObject];
	if (site)
	{
		LCDeviceEditController *editController = [[LCDeviceEditController alloc] initForImportFromARDAtSite:site windowForSheet:[self window]];
		[NSApp beginSheet:[editController window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

- (IBAction) addNewSiteToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer)
	{ 
		LCSiteEditController *editController = [[LCSiteEditController alloc] initForNewSiteAtCustomer:customer]; 
		[NSApp beginSheet:[editController window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

- (IBAction) addNewServiceToDeviceMenuCicked:(NSMenuItem *)menuItem
{
	LCDevice *device = [menuItem representedObject];
	if (device)
	{ [LCServiceEditWindowController beginSheetForNewService:device windowForSheet:[self window] delegate:self]; }
}

- (IBAction) addNewEmailActionToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer) [LCActionEditWindowController beginSheetForNewEmailAction:customer windowForSheet:[self window] delegate:self];
}

- (IBAction) addNewActionToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer) [LCActionEditWindowController beginSheetForNewAction:customer windowForSheet:[self window] delegate:self];
}

- (IBAction) addNewSceneToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer)
	{
		[[LCDocumentEditWindowController alloc] initForNewDocumentWithCustomer:customer type:@"scene" windowForSheet:[self window]];
	}	
}

- (IBAction) addNewVRackToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer)
	{
		[[LCDocumentEditWindowController alloc] initForNewDocumentWithCustomer:customer type:@"vrack" windowForSheet:[self window]];
	}	
}

- (IBAction) addNewGroupToCustomerMenuCicked:(NSMenuItem *)menuItem
{
	LCCustomer *customer = [menuItem representedObject];
	if (customer)
	{
		[[LCGroupEditWindowController alloc] initForNewGroupUnderParent:nil customer:customer windowForSheet:[self window]];
	}	
}


#pragma mark "UI Actions"

- (IBAction) refreshSelectedClicked:(id)sender
{
	/* Context-sensitive refresh. Perform appropriate
	 * refresh action depending on what is selected
	 */
	
	if (!self.treeSelectedObject) return;
	
	if (self.treeSelectedEntity)
	{
		if ([self.treeSelectedEntity device])
		{ [self.treeSelectedEntity.device highPriorityRefresh]; }
		else
		{ [self.treeSelectedEntity.customer highPriorityRefresh]; }
	}	
}

- (IBAction) faultHistoryClicked:(NSMenuItem *)sender
{
	/* Create fault history controller */
	for (LCEntity *entity in self.selectedEntities)
	{
		[[LCFaultHistoryController alloc] initForEntity:entity];
	}
}

- (IBAction) refreshEntityClicked:(NSMenuItem *)sender
{
	/* Perform refresh */
	for (LCEntity *entity in self.selectedEntities)
	{ 
		if ([entity device])
		{ [[entity device] highPriorityRefresh]; }
		else
		{ [[entity customer] highPriorityRefresh]; }
	}
}

- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender
{
	LCEntity *entity;
	NSEnumerator *enumerator = [self.selectedEntities objectEnumerator];
	
	/* Check something is selected */
	if ([self.selectedEntities count] < 1)
	{ return; }
	
	/* Check entities */
	NSString *prevCustomerName = nil;
	while (entity = [enumerator nextObject])
	{
		if (prevCustomerName && ![[[entity customer] name] isEqualToString:prevCustomerName])
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
		prevCustomerName = [[entity customer] name];
	}
	
	/* Open Case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:self.selectedEntities];
}

- (IBAction) viewPropertiesClicked:(NSMenuItem *)sender
{
	/* Check entities */
	for (LCEntity *entity in self.selectedEntities)
	{ [[LCPropertiesViewerController alloc] initWithDictionary:[entity properties]]; }
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, open a browser for the first one */
	if (self.selectedEntities && [self.selectedEntities count] > 0)
 	{ [[LCBrowser2Controller alloc] initWithEntity:[self.selectedEntities objectAtIndex:0]]; }
}

- (IBAction) addNewSiteClicked:(id)sender
{
	LCSiteEditController *editController;
	editController = [[LCSiteEditController alloc] initForNewSiteAtCustomer:[self.selectedEntity customer]];
	[NSApp beginSheet:[editController window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) editSiteClicked:(id)sender
{
	LCSiteEditController *editController;
	editController = [[LCSiteEditController alloc] initWithSiteToEdit:[self.selectedEntity site]];
	[NSApp beginSheet:[editController window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) removeSiteClicked:(id)sender
{
	LCSiteEditController *editController;
	editController = [[LCSiteEditController alloc] initWithSiteToRemove:[self.selectedEntity site] windowForSheet:[self window]];
}

- (IBAction) addNewDeviceClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initForNewDeviceAtSite:[self.selectedEntity site]];
	[NSApp beginSheet:[editController window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) duplicateDeviceClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDeviceToDuplicate:[self.selectedEntity device]];
	[NSApp beginSheet:[editController window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) editDeviceClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDeviceToEdit:[self.selectedEntity device]];
	[NSApp beginSheet:[editController window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) removeDeviceClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDeviceToRemove:[self.selectedEntity device] windowForSheet:[self window]];
}

- (void) entityDoubleClicked:(NSArray *)entities
{
	if ([entities count] > 0)
	{ 
//		[self selectEntity:[entities objectAtIndex:0]]; 
	}
}

- (void) incidentDoubleClicked:(NSArray *)incidents
{
	if ([incidents count] > 0)
	{ 
		[[LCIncidentController alloc] initForIncident:[incidents objectAtIndex:0]];
	}	
}

- (void) deviceOutlineViewDoubleClicked:(NSArray *)selectedObjects
{
	id object;
	for (object in selectedObjects)
	{
		if ([object class] == [LCEntity class])
		{
//			[self selectEntity:object];
		}
		else if ([object class] == [LCIncident class])
		{
			[[LCIncidentController alloc] initForIncident:object];
		}
	}
}

- (IBAction) actionListClicked:(id)sender
{
//	if ([self.selectedEntities count] < 1) return;
//	[[LCActionListWindowController alloc] initForCustomer:[[self.selectedEntities objectAtIndex:0] customer]];	
}

- (IBAction) addNewActionClicked:(id)sender
{
	if ([[self selectedEntity] customer]) 
	{ [LCActionEditWindowController beginSheetForNewAction:[[self selectedEntity] customer] windowForSheet:[self window] delegate:self]; }
}

- (IBAction) addNewEmailActionClicked:(id)sender
{
	if ([[self selectedEntity] customer]) 
	{ [LCActionEditWindowController beginSheetForNewEmailAction:[[self selectedEntity] customer] windowForSheet:[self window] delegate:self]; }
}

- (IBAction) manageActionScriptsClicked:(id)sender
{
	if (treeSelectedEntity)
	{ [LCLithiumSetupWindowController actionScriptSetupControllerForCustomer:[treeSelectedEntity customer]]; }
}

- (IBAction) restartMonitoringProcessClicked:(id)sender
{
	for (LCEntity *entity in self.selectedEntities)
	{
		[entity restartMonitoringProcess];
	}
}

- (IBAction) restartCustomerProcessClicked:(id)sender
{
	[[[self selectedEntity] customer] restartMonitoringProcess];
}

- (IBAction) entityTreeSelectButtonClicked:(id)sender
{
	/* Pop menu */
	[NSMenu popUpContextMenu:browserTreeSelectMenu withEvent:[NSApp currentEvent] forView:entityTreeSelectButton];
}

- (IBAction) manageLicensesClicked:(id)sender
{
	if (treeSelectedEntity)
	{ [LCLithiumSetupWindowController licenseSetupControllerForCustomer:[treeSelectedEntity customer]]; }
}

- (IBAction) reviewActiveTriggersClicked:(id)sender
{
	if ([treeSelectedEntity device])
	{ 
		LCReviewActiveTriggersWindowController *controller = [[LCReviewActiveTriggersWindowController alloc] initForDevice:[treeSelectedEntity device]];
		[controller setParentWindow:[self window]];
		[NSApp beginSheet:[controller window] 
		   modalForWindow:[self window] 
			modalDelegate:self 
		   didEndSelector:nil
			  contextInfo:nil];		
	}
}

- (IBAction) resetTriggerRulesClicked:(id)sender
{
	if ([self.selectedEntities count] < 1) return;
	
	LCResetTriggerRulesWindowController *controller = (LCResetTriggerRulesWindowController *) [[LCResetTriggerRulesWindowController alloc] initForEntity:[self.selectedEntities objectAtIndex:0]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) resetDeviceTriggerRulesClicked:(id)sender
{
	if ([self.selectedEntities count] < 1) return;
	
	LCResetTriggerRulesWindowController *controller = (LCResetTriggerRulesWindowController *) [[LCResetTriggerRulesWindowController alloc] initForEntity:[[self.selectedEntities objectAtIndex:0] device]];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) preferencesClicked:(id)sender
{
	[LCPreferencesController preferencesController];
}

- (IBAction) errorLogClicked:(id)sender
{
	[LCErrorLogWindowController errorLogController];
}

- (IBAction) markDeviceOutOfServiceClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDevice:self.selectedEntity.device toMarkAs:-3];
}

- (IBAction) markDeviceInTestingClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDevice:self.selectedEntity.device toMarkAs:-2];
}

- (IBAction) markDeviceInProductionClicked:(id)sender
{
	LCDeviceEditController *editController;
	editController = [[LCDeviceEditController alloc] initWithDevice:self.selectedEntity.device toMarkAs:0];	
}

- (IBAction) renameSelectedDocumentClicked:(id)sender;
{
	LCDocument *document = (LCDocument *) treeSelectedObject;
	[[LCDocumentEditWindowController alloc] initWithDocumentToEdit:document
														  customer:document.customer
													windowForSheet:[self window]];
}

- (IBAction) deleteSelectedDocumentClicked:(NSMenuItem *)menuItem
{
	LCDocument *document = (LCDocument *) treeSelectedObject;
	[[LCDocumentEditWindowController alloc] initWithDocumentToDelete:treeSelectedObject
															customer:document.customer
													  windowForSheet:[self window]];
}

- (IBAction) addNewGroupToSelectedClicked:(id)sender
{
	LCGroup *parent = nil;
	LCCustomer *customer = nil;
	if ([treeSelectedObject class] == [LCGroup class])
	{
		parent = treeSelectedObject;
		customer = parent.customer;
	}
	else if ([treeSelectedObject class] == [LCBrowserTreeGroupsCustomer class])
	{
		LCBrowserTreeGroupsCustomer *groupCustomer = treeSelectedObject;
		parent = nil;
		customer = groupCustomer.customer;
	}
	
	if (customer)
	{
		[[LCGroupEditWindowController alloc] initForNewGroupUnderParent:parent customer:customer windowForSheet:[self window]];
	}
}

- (IBAction) renameSelectedGroupClicked:(id)sender
{
	if ([treeSelectedObject class] == [LCGroup class])
	{
		LCGroup *group = treeSelectedObject;
		[[LCGroupEditWindowController alloc] initWithGroupToEdit:group customer:group.customer windowForSheet:[self window]];
	}		
}

- (IBAction) deleteSelectedGroupClicked:(id)sender
{
	if ([treeSelectedObject class] == [LCGroup class])
	{
		LCGroup *group = treeSelectedObject;
		[[LCGroupEditWindowController alloc] initWithGroupToDelete:group windowForSheet:[self window]];
	}	
}

- (IBAction) removeSelectedEntityFromGroupClicked:(NSMenuItem *)menuItem
{
	LCGroup *parent = [menuItem representedObject];
	if ([[treeSelectedObject class] isSubclassOfClass:[LCEntity class]])
	{
		[[LCGroupEditWindowController alloc] initWithEntity:treeSelectedObject toRemoveFromGroup:parent windowForSheet:[self window]];
	}
}

- (IBAction) refreshIncidentsClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.activeIncidentsList highPriorityRefresh];
	}
}

- (IBAction) refreshCasesClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.openCasesList highPriorityRefresh];
	}	
}

- (IBAction) refreshServicesClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.serviceList highPriorityRefresh];
	}
}

- (IBAction) refreshProcessesClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.processProfileList highPriorityRefresh];
	}	
}

- (IBAction) refreshDocumentListClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.documentList highPriorityRefresh];
	}		
}

- (IBAction) refreshGroupsClicked:(id)sender
{
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{ 
		[customer.groupTree highPriorityRefresh];
	}
}

- (IBAction) refreshDeploymentClicked:(NSMenuItem *)menuItem
{
	LCCoreDeployment *deployment = [(LCBrowserTreeCoreDeployment *)[menuItem representedObject] deployment];
	[deployment refreshDeployment];
}

#pragma mark "New Toolbar UI Actions"

- (IBAction) addNewClicked:(id)sender
{
	[NSMenu popUpContextMenu:toolbarAddMenu withEvent:[NSApp currentEvent] forView:[[self window] contentView]];
}

- (IBAction) lithiumSetupClicked:(id)sender
{
	[LCLithiumSetupWindowController setupControllerForCustomer:[treeSelectedEntity customer]];
}

- (IBAction) incidentManagerClicked:(id)sender
{
	[[LCIncmgrController alloc] init];
}

- (IBAction) caseSearchClicked:(id)sender
{
	LCCaseSearchController *controller = [[LCCaseSearchController alloc] init];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (IBAction) setupAssistantClicked:(id)sender
{
	LCCustomer *customer = nil;
	if (self.selectedEntity)
	{
		customer = [self.selectedEntity customer];
	}
	else 
	{
		if ([[LCCustomerList masterArray] count] == 1)
		{ customer = [[LCCustomerList masterArray] objectAtIndex:0]; }
	}

	[[LCAssistController alloc] initForCustomer:customer];
}
		
#pragma mark "LITHIUM.Web URL Methods"

- (IBAction) copyWebUrlToClipBoardClicked:(id)sender
{
	/* Copy */
	if ([self.selectedEntity webURLString])
	{
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		NSArray *types = [NSArray arrayWithObject:NSStringPboardType];
		[pb declareTypes:types owner:self];
		[pb setString:[self.selectedEntity webURLString] forType:NSStringPboardType];
	}
}

- (IBAction) openWebUrlInBrowserClicked:(id)sender
{
	/* Open */
	if ([self.selectedEntity webURLString])
	{
		NSURL *url = [NSURL URLWithString:[self.selectedEntity webURLString]];
		[[NSWorkspace sharedWorkspace] openURL:url];
	}
}

#pragma mark "Device Refresh Methods"

- (void) refreshDeviceTimerFired
{
	/* Get selected */
	[[self.selectedEntity device] highPriorityRefresh];
}

#pragma mark "Connect-using Methods"

- (IBAction) connectUsingSSH:(id)sender
{
	/* Get selected */
	[self.selectedEntity connectUsingSSH];
}

- (IBAction) connectUsingTelnet:(id)sender
{
	/* Get selected */
	[self.selectedEntity connectUsingTelnet];
}

- (IBAction) connectUsingWeb:(id)sender
{
	/* Get selected */
	[self.selectedEntity connectUsingWeb];
}

- (IBAction) connectUsingARD:(id)sender
{
	/* Get selected */
	[self.selectedEntity connectUsingARD];
}

#pragma mark "Service Methods"

- (IBAction) addNewServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewService:[self.selectedEntity device]
											windowForSheet:[self window]
												  delegate:self];
}

- (IBAction) addNewHTTPServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewHTTPService:[self.selectedEntity device]
												windowForSheet:[self window]
													  delegate:self];
}

- (IBAction) addNewSMTPServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewSMTPService:[self.selectedEntity device]
												windowForSheet:[self window]
													  delegate:self];
}

- (IBAction) addNewPOPServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewPOPService:[self.selectedEntity device]
											   windowForSheet:[self window]
													 delegate:self];
}

- (IBAction) addNewIMAPServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewIMAPService:[self.selectedEntity device]
												windowForSheet:[self window]
													  delegate:self];
}

- (IBAction) addNewDNSServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForNewDNSService:[self.selectedEntity device]
												windowForSheet:[self window]
													  delegate:self];
}


- (IBAction) manageServiceScriptsClicked:(id)sender
{
}

#pragma mark "Process Profile Methods"

- (IBAction) addNewProcessProfileClicked:(id)sender
{
	[[LCProcessProfileEditWindowController alloc] initForNewProfileMatch:nil device:[self.selectedEntity device] windowForSheet:[self window]];
}

#pragma mark "Browser Tree Methods"

@synthesize browserTree;
@synthesize browserTreeOutlineView;

#pragma mark "Search Methods"

@synthesize searchString;
- (void) setSearchString:(NSString *)string
{
	[searchString release];
	searchString = [string copy];
	if (searchString && [searchString length] > 0)
	{
		[browserTreeOutlineView deselectAll:nil];	// Clear browser tree selection
		if (!self.searchContentController)
		{
			/* Create new Search Controller */
			self.searchContentController = [[LCBrowserSearchContentController alloc] initInBrowser:self];
			self.contentViewController = self.searchContentController;
		}
		searchContentController.searchString = searchString;
	}
	else
	{ 
		/* Remove search string controller */
		self.searchContentController = nil;
		
		/* Reset tree selection (and hence restore content view) */
		id currentSelection = self.treeSelectedObject;
		self.treeSelectedObject = nil;
		self.treeSelectedObject = currentSelection;
	}
}

@synthesize searchContentController;

- (NSPredicate *) customerEnabledFilter
{ return customerEnabledFilter; }

#pragma mark "Feedback Methods"

- (IBAction) sendFeedbackClicked:(id)sender
{
	[[LCFeedbackController alloc] initForWindow:[self window]];
}

#pragma mark "Accessors"

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

- (NSArray *) custSortDescriptors
{ 
	return custSortDescriptors; 
}
- (void) setCustSortDescriptors:(NSArray *)array
{
	if (custSortDescriptors) [custSortDescriptors release];
	custSortDescriptors = [array retain];
}

- (NSArray *) siteSortDescriptors
{ return siteSortDescriptors; }
- (void) setSiteSortDescriptors:(NSArray *)array
{ 
	if (siteSortDescriptors) [siteSortDescriptors release];
	siteSortDescriptors = [array retain];
}

- (NSArray *) deviceSortDescriptors
{ return deviceSortDescriptors; }
- (void) setDeviceSortDescriptors:(NSArray *)array
{ 
	if (deviceSortDescriptors) [deviceSortDescriptors release];
	deviceSortDescriptors = [array retain];
}

- (NSArray *) containerSortDescriptors
{ return containerSortDescriptors; }
- (void) setContainerSortDescriptors:(NSArray *)array
{ 
	if (containerSortDescriptors) [containerSortDescriptors release];
	containerSortDescriptors = [array retain];
}

- (NSArray *) objectSortDescriptors
{ return objectSortDescriptors; }
- (void) setObjectSortDescriptors:(NSArray *)array
{ 
	if (objectSortDescriptors) [objectSortDescriptors release];
	objectSortDescriptors = [array retain];
}

@end

