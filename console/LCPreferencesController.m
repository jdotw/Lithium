//
//  LCPreferencesController.m
//  Lithium Console
//
//  Created by James Wilson on 3/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCPreferencesController.h"
#import "LCCustomerList.h"
#import "LCCoreDeployment.h"

static LCPreferencesController *masterController = nil;

@implementation LCPreferencesController

#pragma mark "Initialisation"

+ (LCPreferencesController *) preferencesController
{
	if (masterController)
	{
		[[masterController window] makeKeyAndOrderFront:self];
		return nil;
	}
	else
	{
		masterController = [[LCPreferencesController alloc] init];
		return masterController; 
	}
}

- (LCPreferencesController *) init
{
	/* Super class init */
	[super initWithWindowNibName:@"PreferencesWindow"];
	
 	/* Demo check */
	demoFlag = NO;
#ifdef DEMO_MODE
	demoFlag = YES;
#endif
	
	/* Build deployment list */
	deployments = [[NSMutableArray arrayWithArray:[[LCCustomerList masterList] dynamicDeployments]] retain];
	[deployments addObjectsFromArray:[[LCCustomerList masterList] staticDeployments]];
	[[LCCustomerList masterList] addObserver:self
								  forKeyPath:@"staticDeployments"
								  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
								  context:NULL];
	[[LCCustomerList masterList] addObserver:self
								  forKeyPath:@"dynamicDeployments"
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];

	/* Window setup */
	[self window];
	[self buildToolbar];
	
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"staticDeploymenyts"];
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"dynamicDeploymenyts"];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self willChangeValueForKey:@"deployments"];
	[deployments removeAllObjects];
	[deployments addObjectsFromArray:[[LCCustomerList masterList] dynamicDeployments]];
	[deployments addObjectsFromArray:[[LCCustomerList masterList] staticDeployments]];
	[self didChangeValueForKey:@"deployments"];	
}

#pragma mark "Register defaults"

+ (void) registerDefaults
{
	NSMutableDictionary *defaults = [NSMutableDictionary dictionary];
	
	/* Auto-refresh */
	[defaults setValue:@"30" forKey:@"refActiveIncidentRefreshInterval"];
	[defaults setValue:@"300" forKey:@"refOpenCaseRefreshInterval"];
	
	/* Connectivity */
	[defaults setValue:@"10" forKey:@"xmlMaxParrallellReqs"];
	[defaults setValue:@"60" forKey:@"xmlHTTPTimeoutSec"];
	[defaults setValue:@"60" forKey:@"graphHTTPTimeoutSec"];

	/* Customer config */
	[defaults setValue:[NSNumber numberWithBool:YES] forKey:PREF_CUSTLIST_DISCOVERY_BOOL];
	[defaults setValue:[NSNumber numberWithBool:YES] forKey:PREF_CUSTLIST_ALERTERROR_BOOL];
	
	/* Inspector */
	[defaults setValue:[NSNumber numberWithBool:NO] forKey:@"inspectorAutoAdjustPanelHeight"];
	[defaults setValue:[NSNumber numberWithBool:NO] forKey:@"inspectorAnimatesAdjustPanelHeight"];
	[defaults setValue:[NSNumber numberWithBool:NO] forKey:@"inspectorDisplayInTopRight"];
	
	/* Notifications */
	[defaults setValue:[NSNumber numberWithBool:NO] forKey:@"dingOnIncident"];
	[defaults setValue:[NSNumber numberWithBool:YES] forKey:@"growlNotifications"];
	
	/* Device Add/Edit */
	[defaults setValue:[NSNumber numberWithInt:1] forKey:@"LCDeviceEditControllerOSXServerProtocol"];
	[defaults setValue:[NSNumber numberWithInt:0] forKey:@"LCDeviceEditControllerOSXClientProtocol"];
	[defaults setValue:@"snmpbasic" forKey:@"LCDeviceEditControllerLastUsedModule"];
	
	/* Browser Tree Indexes */
	[defaults setValue:[NSNumber numberWithInt:1] forKey:@"LCBrowserTree-MONITORED DEVICES-Index"];
	[defaults setValue:[NSNumber numberWithInt:2] forKey:@"LCBrowserTree-GROUPS-Index"];
	[defaults setValue:[NSNumber numberWithInt:3] forKey:@"LCBrowserTree-APPLICATIONS-Index"];
	[defaults setValue:[NSNumber numberWithInt:4] forKey:@"LCBrowserTree-FAULTS-Index"];
	[defaults setValue:[NSNumber numberWithInt:5] forKey:@"LCBrowserTree-DOCUMENTS-Index"];
	[defaults setValue:[NSNumber numberWithInt:6] forKey:@"LCBrowserTree-DISCOVERY-Index"];
	[defaults setValue:[NSNumber numberWithInt:7] forKey:@"LCBrowserTree-XSAN-Index"];
	[defaults setValue:[NSNumber numberWithInt:8] forKey:@"LCBrowserTree-LITHIUM DEPLOYMENTS-Index"];
	

	/* Register */
	[[NSUserDefaults standardUserDefaults] registerDefaults:defaults];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"PreferencesToolbar"];
	[toolbar setAutosavesConfiguration:NO];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];

	/* Core Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Deployments"] autorelease];
	[tbitem setLabel:@"Deployments"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (coreClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"core_dot.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	[toolbar setSelectedItemIdentifier:@"Deployments"];

	/* Customer Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Customers"] autorelease];
	[tbitem setLabel:@"Customers " ];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (customersClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"web_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Console Preferences */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Alerts"] autorelease];
	[tbitem setLabel:@"Alerts    " ];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (consoleClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"event_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Refresh Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Auto-Refresh"] autorelease];
	[tbitem setLabel:@"Auto-Refresh"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (autoRefreshClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Connectivity Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Connectivity"] autorelease];
	[tbitem setLabel:@"Connectivity"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (connectivityClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Server.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
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

#pragma mark "Toolbar Targets"

- (void) coreClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"core"];
}

- (void) customersClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"customers"];
}

- (void) autoRefreshClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"refresh"];
}

- (void) connectivityClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"connectivity"];
}

- (void) consoleClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"console"];
}

#pragma mark "UI Actions"

- (IBAction) applyURLClicked:(id)sender
{
	/* Refresh customer list */
	[LCCustomerList masterRefresh];	
}

- (IBAction) addCoreClicked:(id)sender
{
	/* Window Setup */
	[addCoreAddressField setStringValue:@""];
	[addCorePortField setStringValue:@"51180"];
	[addCoreOKButton setTitle:@"Add"];
	[[self window] makeFirstResponder:addCoreAddressField];
	
	/* Begin Sheet */
	[NSApp beginSheet:coreAddSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (void) coreDeploymentDoubleClicked:(NSArray *)selectedObjects
{
	/* Get selected to edit */
	if ([selectedObjects count] < 1) return;	
	editDeployment = [[selectedObjects objectAtIndex:0] retain];
	
	/* Window Set */
	[addCoreAddressField setStringValue:editDeployment.address];
	[addCorePortField setStringValue:[NSString stringWithFormat:@"%i", editDeployment.port]];
	[addCoreOKButton setTitle:@"Save"];

	/* Begin Sheet */
	[[self window] makeFirstResponder:addCoreAddressField];
	[NSApp beginSheet:coreAddSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) removeCoreClicked:(id)sender
{
	/* Remove from array */
	NSEnumerator *selectedEnum = [[coreTreeController selectedObjects] objectEnumerator];
	LCCoreDeployment *deployment;
	while (deployment = [selectedEnum nextObject])
	{
		if (![deployment isDynamic])
		{ 
			int index = [[[self customerList] staticDeployments] indexOfObject:deployment];
			[[self customerList] removeObjectFromStaticDeploymentsAtIndex:index]; 
		}
	}
}

- (IBAction) addCoreAddClicked:(id)sender
{
	/* Check to see if this is an add or edit */
	BOOL deploymentIsNew;
	LCCoreDeployment *deployment;
	if (editDeployment)
	{
		/* Edit */
		deployment = editDeployment;
		[editDeployment release];
		editDeployment = nil;
		deploymentIsNew = NO;
	}
	else
	{
		/* Add */
		deployment = [[[LCCoreDeployment alloc] init] autorelease];
		deploymentIsNew = YES;
	}

	/* Set Values */
	deployment.address = [addCoreAddressField stringValue];
	deployment.port = [[addCorePortField stringValue] intValue];
	deployment.useSSL = addCoreUseSSL;
	
	/* Add */
	if (deploymentIsNew)
	{ [[self customerList] insertObject:deployment inStaticDeploymentsAtIndex:[[[self customerList] staticDeployments] count]]; }
	
	/* Close sheet */
	[NSApp endSheet:coreAddSheet];
	[coreAddSheet close];
}

- (IBAction) addCoreCancelClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:coreAddSheet];
	[coreAddSheet close];
}

@synthesize addCoreUseSSL;
- (void) setAddCoreUseSSL:(BOOL)value
{
	addCoreUseSSL = value;
	if (addCoreUseSSL && [[addCorePortField stringValue] intValue] == 51180)
	{ [addCorePortField setStringValue:@"51143"]; }
	else if (!addCoreUseSSL && [[addCorePortField stringValue] intValue] == 51143)
	{ [addCorePortField setStringValue:@"51180"]; }
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	masterController = nil;
	[self autorelease];
}

#pragma mark "Accessor Methods"

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

- (BOOL) useDiscovery
{ return [[NSUserDefaults standardUserDefaults] boolForKey:PREF_CUSTLIST_DISCOVERY_BOOL]; }
- (void) setUseDiscovery:(BOOL)flag
{ 
	[[NSUserDefaults standardUserDefaults] setBool:flag forKey:PREF_CUSTLIST_DISCOVERY_BOOL]; 
	[[NSUserDefaults standardUserDefaults] synchronize];

	/* Refresh customer list */
	[LCCustomerList masterRefresh];	
}

- (BOOL) alertOnError
{ return [[NSUserDefaults standardUserDefaults] boolForKey:PREF_CUSTLIST_ALERTERROR_BOOL]; }
- (void) setAlertOnError:(BOOL)flag
{ 
	[[NSUserDefaults standardUserDefaults] setBool:flag forKey:PREF_CUSTLIST_ALERTERROR_BOOL]; 
	[[NSUserDefaults standardUserDefaults] synchronize];
}


- (BOOL) demoFlag
{ return demoFlag;} 

- (NSArray *) deployments
{ return deployments; }

@end
