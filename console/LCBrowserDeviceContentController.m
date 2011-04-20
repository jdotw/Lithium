//
//  LCBrowserDeviceContentController.m
//  Lithium Console
//
//  Created by James Wilson on 23/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserDeviceContentController.h"

#import "LCObjectTree.h"
#import "LCMetricGraphDocument.h"
#import "LCFaultHistoryController.h"
#import "LCPropertiesViewerController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCCaseController.h"
#import "LCServiceEditWindowController.h"
#import "LCProcessProfileEditWindowController.h"
#import "LCDeviceEditController.h"
#import "LCConsoleController.h"

@interface LCBrowserDeviceContentController (private)

- (void) clearProcessMenu:(NSMenu *)menu;
- (void) buildProcessMenu:(NSMenu *)menu;

@end

@implementation LCBrowserDeviceContentController

#pragma mark "Constructors"

- (id) initWithDevice:(LCDevice *)initDevice inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [self initWithNibName:@"DeviceContent" bundle:nil];
	
	if (self)
	{
		/* Set/Create objects */
		self.browser = initBrowser;
		self.device = initDevice;
		[self.device highPriorityRefresh];
		NSTimeInterval autoRefreshInterval;
		if (initDevice.refreshInterval > 0.0)
		{ autoRefreshInterval = (initDevice.refreshInterval * 0.5); }
		else
		{ autoRefreshInterval = 30.0; }
		refreshTimer = [[NSTimer scheduledTimerWithTimeInterval:autoRefreshInterval
														 target:self 
													   selector:@selector(refreshTimerFired:) 
													   userInfo:nil 
														repeats:YES] retain];
		
		self.containerTree = [[[LCContainerTree alloc] initWithDevice:device] autorelease];
		
		/* Set initial selection */
		self.selectedEntities = [NSArray arrayWithObject:self.device];

		/* Load the NIB */
		[self loadView];
		[self buildProcessMenu:[[LCConsoleController masterController] processMonitorMenu]];
		
		/* Inspector setup */
		[inspectorController bind:@"target" toObject:self withKeyPath:@"selectedEntity" options:nil];
		
		/* Stadium Setup */
		[stadiumController bind:@"target" toObject:self withKeyPath:@"device" options:nil];
		stadiumController.title = [NSString stringWithFormat:@"%@ (%@)", device.displayString, device.ipAddress];

		/* Expand Container Tree */
		int i;
		for (i=0; i < [containerOutlineView numberOfRows]; i++)
		{ [containerOutlineView expandItem:[containerOutlineView itemAtRow:i]];	}	
		
		/* Observe Container and Object Tree Selection */
		[containerTreeController addObserver:self 
								  forKeyPath:@"selection" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
									 context:nil];
		[objectTreeController addObserver:self 
							   forKeyPath:@"selection" 
								  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								  context:nil];
		[stadiumController addObserver:self
							forKeyPath:@"selectedEntity"
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							   context:nil];
		
		/* Observe change in Container and Object OutlineView Responder */
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(firstResponderChanged:) 
													 name:@"WillBecomeFirstResponder" 
												   object:containerOutlineView];
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(firstResponderChanged:) 
													 name:@"WillBecomeFirstResponder" 
												   object:objectOutlineView];
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(firstResponderChanged:) 
													 name:@"WillBecomeFirstResponder" 
												   object:browser.browserTreeOutlineView];
		
		/* Listen for device refresh */
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(deviceRefreshFinished:)
													 name:@"EntityRefreshFinished"
												   object:device];
	}
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	if (availAttachedWindow) [availAttachedWindow closeAttachedWindow];
	[self clearProcessMenu:[[LCConsoleController masterController] processMonitorMenu]];
	[inspectorController unbind:@"target"];
	[containerTreeController removeObserver:self forKeyPath:@"selection"];
	[objectTreeController removeObserver:self forKeyPath:@"selection"];
	[stadiumController removeObserver:self forKeyPath:@"selectedEntity"];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"WillBecomeFirstResponder" object:containerOutlineView];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"WillBecomeFirstResponder" object:objectOutlineView];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"WillBecomeFirstResponder" object:browser.browserTreeOutlineView];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"EntityRefreshFinished" object:device];
	[refreshTimer invalidate];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[device release];
	[containerTree release];
	[objectTree release];
	[refreshTimer release];
	[selectedEntities release];
	
	[super dealloc];
}

#pragma mark - Attached Window Delegate

- (void) attachedWindowDidClose:(LCAttachedWindow *)attachedWindow
{
	if (attachedWindow == availAttachedWindow)
	{
		[availAttachedWindow release];
		availAttachedWindow = nil;
	}
}

#pragma mark "Selection (KVO Observable)"

- (void) updateEntitySelectionUsingContainerTree
{
	/* Container Tree Selection Changed */
	if ([[containerTreeController selectedObjects] count] > 0)
	{
		/* Set selection */
		self.selectedEntities = [containerTreeController selectedObjects]; 
	}	
	else
	{
		if ([self.window firstResponder] == containerOutlineView)
		{ self.selectedEntities = nil; }
		self.selectedEntities = nil; 
	}	
}

- (void) updateEntitySelectionUsingObjectTree
{
	/* Container Tree Selection Changed */
	if ([[objectTreeController selectedObjects] count] > 0)
	{
		/* Set selection */
		NSMutableArray *selectedObjects = [NSMutableArray array];
		for (LCObjectTreeItem *item in [objectTreeController selectedObjects])
		{ [selectedObjects addObject:item.object]; }
		self.selectedEntities = selectedObjects; 
	}	
	else
	{
		if ([self.window firstResponder] == objectOutlineView)
		{ self.selectedEntities = nil; }
	}	
}

- (void) updateEntitySelectionUsingGraphStadium
{
	if (stadiumController.selectedEntity)
	{
		if ([stadiumController.selectedEntity metric])
		{ self.selectedEntities = [NSArray arrayWithObject:[stadiumController.selectedEntity object]]; }
		else
		{ self.selectedEntities = [NSArray arrayWithObject:stadiumController.selectedEntity]; }
	}
	else
	{
		self.selectedEntities = nil;
	}
}

- (void) updateEntitySelectionUsingDevice
{
	self.selectedEntities = [NSArray arrayWithObject:self.device]; 
}
		
@synthesize selectedEntity;

@synthesize selectedEntities;
- (void) setSelectedEntities:(NSArray *)value
{
	[selectedEntities release];
	selectedEntities = [value copy];
	
	/* Check to see if a selection was specified */
	if ([selectedEntities count] > 0)
	{ 
		/* Selection specified, use it. */
		self.selectedEntity = [selectedEntities objectAtIndex:0];
		
		/* Update stadium flow */
		if (!doNotScrollFlow)
		{
			if (stadiumController.flowController.target != [self.selectedEntity container])
			{ stadiumController.flowController.target = [self.selectedEntity container]; }
			if ([self.selectedEntity object])
			{ [stadiumController.flowController scrollToObject:[self.selectedEntity object]]; }
		}
	}
	else
	{ 
		/* No selecton specified, use device */
		self.selectedEntities = [NSArray arrayWithObject:self.device];
		stadiumController.flowController.target = nil;
	}
}

- (void) selectEntity:(LCEntity *)entity
{
	if ([entity container])
	{
		[containerOutlineView selectContainer:[entity container]];
	}
	if ([entity object])
	{
		[objectOutlineView selectObject:[entity object]];
	}
	
//	if ([entity type] >= 5)
//	{ [[self window] makeFirstResponder:objectOutlineView]; }
//	else
//	{ [[self window] makeFirstResponder:containerOutlineView]; }
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == containerTreeController)
	{
		/* Update Entity Selection */
		[self updateEntitySelectionUsingContainerTree];
		
		/* Update Object Tree */
		if ([[containerTreeController selectedObjects] count] > 0)
		{
			/* Create new object tree */
			if ([[[[containerTreeController selectedObjects] objectAtIndex:0] class] isSubclassOfClass:[LCContainer class]])
			{
				LCContainer *container = [[containerTreeController selectedObjects] objectAtIndex:0];
				self.objectTree = [[[LCObjectTree alloc] initWithContainer:container] autorelease];
				[objectOutlineView expandAllItemsUsingPreferences];
			}
			else if ([[[[containerTreeController selectedObjects] objectAtIndex:0] class] isSubclassOfClass:[LCObject class]])
			{
				LCObject *object = [[containerTreeController selectedObjects] objectAtIndex:0];
				self.objectTree = [[[LCObjectTree alloc] initWithObject:object] autorelease];
				[objectOutlineView expandAllItemsUsingPreferences];				
			}
		}	
		else
		{
			self.objectTree = nil; 
		}	
	}
	else if (object == objectTreeController)
	{
		[self updateEntitySelectionUsingObjectTree];
	}
	else if (object == stadiumController)
	{
		LCObject *selectedObject = (LCObject *) stadiumController.selectedEntity;
		if (selectedObject)
		{ 
			if (selectedObject.container) 
			{
				[containerOutlineView selectContainer:selectedObject.container];
				[[self window] makeFirstResponder:containerOutlineView];
			}
			if (selectedObject.object) 
			{
				[objectOutlineView selectObject:selectedObject.object];
				[[self window] makeFirstResponder:objectOutlineView];
			}
		}
		[self updateEntitySelectionUsingGraphStadium];
	}
}

- (void) firstResponderChanged:(NSNotification *)note
{
	if ([note object] == containerOutlineView && [[containerTreeController selectedObjects] count] > 0)
	{ [self updateEntitySelectionUsingContainerTree]; }
	else if ([note object] == objectOutlineView && [[objectTreeController selectedObjects] count] > 0)
	{ [self updateEntitySelectionUsingObjectTree]; }
	else if ([note object] == browser.browserTreeOutlineView)
	{ [self updateEntitySelectionUsingDevice]; }
}

- (void) deviceRefreshFinished:(NSNotification *)note
{
	/* Update stadium */
	[stadiumController updateControllersAndLayers];
	
	/* Update menus */
	[self buildProcessMenu:[[LCConsoleController masterController] processMonitorMenu]];
	
	/* Check availability */
	if (!availAttachedWindowShown)
	{
		BOOL showAvailPopup = NO;
		[availAttachedWindowProtocolLabel setStringValue:@""];
		for (LCObject *availObj in [[self.device childNamed:@"avail"] children])
		{
			if ([[availObj name] isEqualToString:@"master"]) continue;
			NSString *rawOKpercentValue = [availObj rawValueForMetricNamed:@"ok_pc"];
			if (rawOKpercentValue && [rawOKpercentValue floatValue] < 50.)
			{
				NSString *protocolString;
				if ([[availAttachedWindowProtocolLabel stringValue] length] > 0) 
				{ protocolString = [[availAttachedWindowProtocolLabel stringValue] stringByAppendingFormat:@", %@", [availObj desc]]; }
				else 
				{ protocolString = [availObj desc]; }
				[availAttachedWindowProtocolLabel setStringValue:protocolString];
				showAvailPopup = YES;
			}
		}
		if (showAvailPopup)
		{
			NSPoint pointRelToButton = NSMakePoint(NSMidX([deviceSettingsButton bounds]), NSMinY([deviceSettingsButton bounds]));
			availAttachedWindow = [[LCAttachedWindow alloc] initWithView:availAttachedWindowView
														 attachedToPoint:[deviceSettingsButton convertPoint:pointRelToButton
																									 toView:nil]
																inWindow:[self window]
																  onSide:MAPositionTop
															  atDistance:0.0];
			[availAttachedWindow setDelegate:self];
			[availAttachedWindow setAlertStyle:NSCriticalAlertStyle];
			[[deviceSettingsButton window] addChildWindow:availAttachedWindow
												  ordered:NSWindowAbove];
			availAttachedWindowShown = YES;
		}
	}
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	/* 
	 * Monitored Entity
	 */
	
	if (action == @selector(graphSelectedClicked:)) 
	{ if ([selectedEntity container]) return YES; }

	else if (action == @selector(faultHistoryClicked:)) 
	{ if ([selectedEntity customer]) return YES; }

	else if (action == @selector(analyseSelectedClicked:)) 
	{ if ([selectedEntity object]) return YES; }
	
	else if (action == @selector(openCaseForSelectedClicked:))
	{ if ([selectedEntity customer] && [[selectedEntity customer] userIsNormal] && [self.selectedEntities count] > 0) return YES; }

	else if (action == @selector(triggerTuningClicked:))
	{ if ([selectedEntity object] && [[selectedEntity customer] userIsAdmin] && [self.selectedEntity object]) return YES; }

	else if (action == @selector(copyWebUrlToClipBoardClicked:))
	{ if ([selectedEntity customer]) return YES; }
	
	else if (action == @selector(openWebUrlInBrowserClicked:))
	{ if ([selectedEntity customer]) return YES; }

	else if (action == @selector(browseToSelectedClicked:))
	{ if (selectedEntity) return YES; }
	
	else if (action == @selector(faultHistoryClicked:))
	{ if (selectedEntity) return YES; }
	
	else if ([NSStringFromSelector(action) hasPrefix:@"servicePopUpAdd"])
	{ return YES; }

	else if (action == @selector(servicePopUpEditClicked:))
	{ return YES; }

	else if (action == @selector(servicePopUpRemoveClicked:))
	{ return YES; }

	else if (action == @selector(processPopUpAddClicked:))
	{ return YES; }

	else if (action == @selector(processPopUpEditClicked:))
	{ return YES; }

	else if (action == @selector(processPopUpRemoveClicked:))
	{ return YES; }
	
	else if (action == @selector(editServiceClicked:) && self.selectedEntity.type == 5 && [[self.selectedEntity.parent name] isEqualToString:@"service"])
	{ return YES; }

	else if (action == @selector(removeServiceClicked:) && self.selectedEntity.type == 5 && [[self.selectedEntity.parent name] isEqualToString:@"service"])
	{ return YES; }

	else if (action == @selector(editProcessProfileClicked:) && self.selectedEntity.type == 5 && [[self.selectedEntity.parent name] isEqualToString:@"procpro"])
	{ return YES; }
	
	else if (action == @selector(removeProcessProfileClicked:) && self.selectedEntity.type == 5 && [[self.selectedEntity.parent name] isEqualToString:@"procpro"])
	{ return YES; }
	
	else if (action == @selector(adjustTriggersForObjectMenuItem:))
	{ return YES; }
	
	return NO;
}

#pragma mark "UI Actions"

- (IBAction) refreshDeviceClicked:(id)sender
{
	[self.device highPriorityRefresh];
}

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, and graph them */
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = self.selectedEntities;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (IBAction) faultHistoryClicked:(NSMenuItem *)sender
{
	/* Create fault history controller */
	for (LCEntity *entity in self.selectedEntities)
	{ [[LCFaultHistoryController alloc] initForEntity:entity]; }
}

- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender
{
	/* Open Case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:self.selectedEntities];
}

- (IBAction) viewPropertiesClicked:(NSMenuItem *)sender
{
	/* Check entities */
	for (LCEntity *entity in self.selectedEntities)
	{ [[LCPropertiesViewerController alloc] initWithDictionary:[entity properties]]; }
}

- (IBAction) metricHistoryClicked:(id)sender
{
	for (LCEntity *entity in self.selectedEntities)
	{
		if ([[entity typeInteger] intValue] != 6)
		{ continue; }
		[[LCMetricHistoryWindowController alloc] initWithMetric:(LCMetric *)entity];
	}
}

- (IBAction) triggerTuningClicked:(id)sender
{
	/* Open trigger tuning window */
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[self.selectedEntity object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) adjustTriggersForObjectMenuItem:(NSMenuItem *)item
{
	/* Open trigger tuning window from a menu item*/
	LCObject *obj = (LCObject *) [item representedObject];
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:obj];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
	
}

- (IBAction) analyseSelectedClicked:(id)sender
{
	/* Open analysis window */
	[[LCMetricAnalysisWindowController alloc] initWithObject:[self.selectedEntity object]];
}

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

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, open a browser for the first one */
	if (self.selectedEntity)
 	{ [[[LCBrowser2Controller alloc] initWithEntity:self.selectedEntity] autorelease]; }
}

- (IBAction) incidentPopUpClicked:(id)sender
{
	/* Clear Old */
	while ([[incidentPopUpAdjustMenu itemArray] count])
	{ [incidentPopUpAdjustMenu removeItemAtIndex:0]; }
	
	/* Build container/object tree of items to adjust */
	for (LCContainer *cnt in self.device.children)
	{
		NSMenuItem *cntMenuItem = [[[NSMenuItem alloc] initWithTitle:cnt.desc action:nil keyEquivalent:@""] autorelease];
		if (cnt.children.count > 1)
		{
			NSMenu *cntMenu = [[[NSMenu alloc] initWithTitle:cnt.desc] autorelease];
			[cntMenuItem setSubmenu:cntMenu];
			for (LCObject *obj in cnt.children)
			{
				NSMenuItem *objItem = [[[NSMenuItem alloc] initWithTitle:obj.desc
																  action:@selector(adjustTriggersForObjectMenuItem:)
														   keyEquivalent:@""] autorelease];
				[objItem setTarget:self];
				[objItem setRepresentedObject:obj];
				[cntMenu addItem:objItem];
			}
		}
		else if (cnt.children.count == 1)
		{
			[cntMenuItem setTarget:self];
			[cntMenuItem setAction:@selector(adjustTriggersForObjectMenuItem:)];
			[cntMenuItem setRepresentedObject:[cnt.children objectAtIndex:0]];
		}
		else if (cnt.children.count == 0)
		{
			cntMenuItem = nil;
		}
		if (cntMenuItem) [incidentPopUpAdjustMenu addItem:cntMenuItem];
	}
	
	/* Show menu */
	[NSMenu popUpContextMenu:incidentPopUpMenu
				   withEvent:[NSApp currentEvent]
					 forView:incidentPopUpButton];	
}

- (IBAction) servicePopUpClicked:(id)sender
{
	/* Remove old items */
	while ([[[serviceMenuEditItem submenu] itemArray] count] > 0)
	{ [[serviceMenuEditItem submenu] removeItemAtIndex:0]; }
	while ([[[serviceMenuRemoveItem submenu] itemArray] count] > 0)
	{ [[serviceMenuRemoveItem submenu] removeItemAtIndex:0]; }
	
	/* Add items per service */
	for (LCService *service in device.services)
	{
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:service.desc 
													  action:@selector(servicePopUpEditClicked:)
											   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:service];
		[[serviceMenuEditItem submenu] insertItem:item atIndex:[[[serviceMenuEditItem submenu] itemArray] count]];
		[item autorelease];

		item = [[NSMenuItem alloc] initWithTitle:service.desc 
										  action:@selector(servicePopUpRemoveClicked:)
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:service];
		[[serviceMenuRemoveItem submenu] insertItem:item atIndex:[[[serviceMenuRemoveItem submenu] itemArray] count]];
		[item autorelease];
	}		

	/* Show menu */
	[NSMenu popUpContextMenu:serviceMenu
				   withEvent:[NSApp currentEvent]
					 forView:serviceMenuButton];
}

- (IBAction) servicePopUpAddClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewService:self.device
											windowForSheet:[self window]
												  delegate:self];
}

- (IBAction) servicePopUpAddHTTPClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewHTTPService:self.device
												windowForSheet:[self window]
													  delegate:self];
}

- (IBAction) servicePopUpAddDNSClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewDNSService:self.device
											   windowForSheet:[self window]
													 delegate:self];
}

- (IBAction) servicePopUpAddSMTPClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewSMTPService:self.device
												windowForSheet:[self window]
													  delegate:self];
}

- (IBAction) servicePopUpAddIMAPClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewIMAPService:self.device
												windowForSheet:[self window]
													  delegate:self];
}


- (IBAction) servicePopUpAddPOPClicked:(NSMenuItem *)sender
{
	[LCServiceEditWindowController beginSheetForNewPOPService:self.device
											   windowForSheet:[self window]
													 delegate:self];
}

- (IBAction) servicePopUpEditClicked:(NSMenuItem *)sender
{
	LCService *service = [sender representedObject];
	[LCServiceEditWindowController beginSheetForServiceToEdit:service.object windowForSheet:[self window] delegate:self];
}

- (IBAction) servicePopUpRemoveClicked:(NSMenuItem *)sender
{
	LCService *service = [sender representedObject];
	[LCServiceEditWindowController beginSheetForServiceToDelete:service.object windowForSheet:[self window]];
}

- (void) buildProcessMenuForAdd:(NSMenu *)addMenu edit:(NSMenu *)editMenu remove:(NSMenu *)removeMenu
{
	/* Add item per process profile (edit/remove) */
	for (LCProcessProfile *procProfile in device.procProfiles)
	{
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:procProfile.desc
													  action:@selector(processPopUpEditClicked:)
											   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:procProfile];
		[editMenu insertItem:item atIndex:[[editMenu itemArray] count]];
		[item autorelease];
		
		item = [[NSMenuItem alloc] initWithTitle:procProfile.desc 
										  action:@selector(processPopUpRemoveClicked:)
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:procProfile];
		[removeMenu insertItem:item atIndex:[[removeMenu itemArray] count]];
		[item autorelease];
	}	
	
	/* Build Process Add Menu */
	NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:@"Other..."
												  action:@selector(processPopUpAddClicked:)
										   keyEquivalent:@""];
	[item setTarget:self];
	[addMenu insertItem:item atIndex:[[addMenu itemArray] count]];
	[item autorelease];
	if (device.procNames.count > 0)
	{
		item = [NSMenuItem separatorItem];
		[addMenu insertItem:item atIndex:[[addMenu itemArray] count]];
	}
	for (NSString *processName in device.procNames)
	{
		item = [[NSMenuItem alloc] initWithTitle:processName
										  action:@selector(processPopUpAddClicked:)
								   keyEquivalent:@""];
		[item setTarget:self];
		[item setRepresentedObject:processName];
		[addMenu insertItem:item atIndex:[[addMenu itemArray] count]];
		[item autorelease];
	}	
	
}

- (IBAction) processPopUpClicked:(id)sender
{
	/* Remove old items */
	while ([[[processMenuEditItem submenu] itemArray] count] > 0)
	{ [[processMenuEditItem submenu] removeItemAtIndex:0]; }
	while ([[[processMenuRemoveItem submenu] itemArray] count] > 0)
	{ [[processMenuRemoveItem submenu] removeItemAtIndex:0]; }
	while ([[[processMenuAddItem submenu] itemArray] count] > 0)
	{ [[processMenuAddItem submenu] removeItemAtIndex:0]; }	

	/* Build menus */
	[self buildProcessMenuForAdd:[processMenuAddItem submenu]
							edit:[processMenuEditItem submenu]
						  remove:[processMenuRemoveItem submenu]];
	
	/* Show Menu */
	[NSMenu popUpContextMenu:processMenu
				   withEvent:[NSApp currentEvent]
					 forView:processMenuButton];
	
}


- (IBAction) processPopUpAddClicked:(NSMenuItem *)sender
{
	[[LCProcessProfileEditWindowController alloc] initForNewProfileMatch:[sender representedObject] device:self.device windowForSheet:[self window]];
}

- (IBAction) processPopUpEditClicked:(NSMenuItem *)sender
{
	LCProcessProfile *profile = [sender representedObject];
	[[LCProcessProfileEditWindowController alloc] initWithProfileToEdit:profile device:self.device windowForSheet:[self window]];
}

- (IBAction) processPopUpRemoveClicked:(NSMenuItem *)sender
{
	LCProcessProfile *profile = [sender representedObject];
	[[LCProcessProfileEditWindowController alloc] initWithProfileToDelete:profile device:self.device windowForSheet:[self window]];
}

- (IBAction) processPopUpEnableMonitoringClicked:(id)sender
{
	LCDeviceEditController *controller;
	controller = [[LCDeviceEditController alloc] initWithDeviceToEdit:self.device];
	controller.monitorProcessList = YES;
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil]; 
}

- (IBAction) editServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForServiceToEdit:[self.selectedEntity object] windowForSheet:[self window] delegate:self];
}

- (IBAction) removeServiceClicked:(id)sender
{
	[LCServiceEditWindowController beginSheetForServiceToDelete:[self.selectedEntity object] windowForSheet:[self window]];
}

- (IBAction) editProcessProfileClicked:(id)sender
{
	LCProcessProfile *procProfile = nil;
	for (procProfile in self.device.procProfiles)
	{
		if (procProfile.profileID == [[[self.selectedEntity object] name] intValue]) break;
	}
	[[LCProcessProfileEditWindowController alloc] initWithProfileToEdit:procProfile 
																 device:self.device
														 windowForSheet:[self window]];
}

- (IBAction) removeProcessProfileClicked:(id)sender
{
	LCProcessProfile *procProfile = nil;
	for (procProfile in self.device.procProfiles)
	{
		if (procProfile.profileID == [[[self.selectedEntity object] name] intValue]) break;
	}
	[[LCProcessProfileEditWindowController alloc] initWithProfileToDelete:procProfile
																   device:self.device
														   windowForSheet:[self window]];
}

- (IBAction) editDeviceClicked:(id)sender
{
	if (availAttachedWindow)
	{
		[availAttachedWindow closeAttachedWindow];
	}
	
	LCDeviceEditController *controller = [[LCDeviceEditController alloc] initWithDeviceToEdit:self.device];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[browser window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

#pragma mark "Device Refresh"

- (void) refreshTimerFired:(NSTimer *)timer
{
	[self.device normalPriorityRefresh];
}

#pragma mark "Process Menu Methods"

- (void) clearProcessMenu:(NSMenu *)menu
{
	for (NSMenuItem *item in [menu itemArray])
	{ [menu removeItem:item]; }	
}

- (void) buildProcessMenu:(NSMenu *)menu
{
	/* Clear old */
	[self clearProcessMenu:menu];
	
	/* Loop through actions */
	NSMenuItem *item;	
	if (device.willUseSNMP)
	{
		if (device.monitorProcessList)
		{
			/* Create Add Menu */
			item = [[[NSMenuItem alloc] initWithTitle:@"Add Profile to Monitor Process"
											   action:nil
										keyEquivalent:@""] autorelease];
			[menu insertItem:item atIndex:[[menu itemArray] count]];			
			NSMenu *addSubMenu = [[NSMenu alloc] init];
			[item setSubmenu:addSubMenu];
			
			
			/* Separator */
			item = [NSMenuItem separatorItem];
			[menu insertItem:item atIndex:[[menu itemArray] count]];
						
			/* Create Edit Menu */
			item = [[[NSMenuItem alloc] initWithTitle:@"Edit Monitored Process Profile"
											   action:nil
										keyEquivalent:@""] autorelease];
			[menu insertItem:item atIndex:[[menu itemArray] count]];
			NSMenu *editSubMenu = [[NSMenu alloc] init];
			[item setSubmenu:editSubMenu];
			
			/* Create Remove Menu */
			item = [[[NSMenuItem alloc] initWithTitle:@"Remove Monitored Process Profile"
											   action:nil
										keyEquivalent:@""] autorelease];
			[menu insertItem:item atIndex:[[menu itemArray] count]];
			NSMenu *removeSubMenu = [[NSMenu alloc] init];
			[item setSubmenu:removeSubMenu];
			
			/* Build menu contents */
			[self buildProcessMenuForAdd:addSubMenu
									edit:editSubMenu
								  remove:removeSubMenu];
		}
		else
		{
			item = [[[NSMenuItem alloc] initWithTitle:@"Enable Process Monitoring"
											  action:@selector(processPopUpEnableMonitoringClicked:)
									   keyEquivalent:@""] autorelease];
			[item setTarget:self];
			[menu insertItem:item atIndex:[[menu itemArray] count]];
		}
	}
	else
	{
		item = [[[NSMenuItem alloc] initWithTitle:@"(Only supported with SNMP Monitoring)"
										   action:nil
									keyEquivalent:@""] autorelease];
		[menu insertItem:item atIndex:[[menu itemArray] count]];		
	}
}

#pragma mark "Container and Object Menus"

- (void)menuWillOpen:(NSMenu *)menu
{
	if (menu == objectMenu)
	{
		NSMutableString *string = [NSMutableString stringWithFormat:@"%@", [selectedEntity.container displayString]];
		if (selectedEntity.object && ([[selectedEntity.object displayString] isEqualToString:@"Master"] || [[selectedEntity.object displayString] isEqualToString:[selectedEntity.container displayString]]))
		{ [string appendString:@" Object"]; }
		else if (selectedEntity.object)
		{ [string appendFormat:@" %@", [selectedEntity.object displayString]]; }
		[objectMenuTitleItem setTitle:string];
	}
	else if (menu == containerMenu)
	{
		[containerMenuTitleItem setTitle:[selectedEntity.container displayString]];
	}
}


#pragma mark "Properties"

@synthesize browser;
@synthesize device;
@synthesize containerTree;
@synthesize objectTree;
- (NSWindow *) window
{ 
	NSResponder *nr = self;
	while ((nr = [nr nextResponder]))
	{ if ([[nr class] isSubclassOfClass:[NSWindow class]]) return (NSWindow *) nr; }
	return nil;
}
- (CGFloat) preferredFixedComponentHeight
{ return 320.0; }

@synthesize doNotScrollFlow;

@end
