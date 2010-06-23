//
//  MBDocumentWindowController.m
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBDocumentWindowController.h"
#import "MBMib.h"
#import "MBContainer.h"
#import "MBModuleUploadRequest.h"
#import "MBTrigger.h"
#import "MBRateMetric.h"
#import "MBPercentMetric.h"
#import "UKCrashReporter.h"
#import "LCFeedbackController.h"

@implementation MBDocumentWindowController

#pragma mark Init

- (MBDocumentWindowController *) init
{
	/* Set Properties */
	self.searchTabTag = @"browse";
	
	/* Load NIB */
	[super initWithWindowNibName:@"ModuleDocument"];
	[self window];
	
	/* Setup Window */
	[self buildToolbar];
	
	/* Show window */
	[[self window] makeKeyAndOrderFront:self];
	
	/* Check for Crashes */
	UKCrashReporterCheckForCrash (self);
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Window Delegates"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Autorelease */
	[objectController setContent:nil];
	[self autorelease];
}

#pragma mark "Toolbar Methods"

- (IBAction) dummyClicked:(id)sender
{
	
}

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
	
	/* Load MIB */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Load MIB"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (loadMIBClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"open_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Scan Device */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Scan Device"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (scanDeviceClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"view_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Add Container */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Add Container"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (addNewContainerClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"product_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Metric */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Add Dynamic Metric"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (newDynamicMetricClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"marker_toolbar_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Triggers */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Add Trigger"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction: @selector (newTriggerClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"effects_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Upload to core */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Export"] autorelease];
	[tbitem setLabel:@"Upload to Core"];
	[tbitem setTarget:self];
	[tbitem setAction:@selector (exportToXMLClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"next_48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Search */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Search"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:searchView];
	[tbitem setMinSize:[searchView bounds].size];
	[tbitem setMaxSize:[searchView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
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


#pragma mark Device Scanning Methods

@synthesize walk;

- (IBAction) scanDeviceClicked:(id)sender
{
	/* Create walk */
	self.walk = [[MBSnmpWalk new] autorelease];
	self.walk.document = [self document];
	self.walk.delegate = self;

	/* Open scan device sheet */
	[NSApp beginSheet:scanDeviceSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	[scanDeviceSheet makeFirstResponder:scanDeviceAddressField];
}

- (IBAction) scanDeviceScanClicked:(id)sender
{
	/* Create walk object and begin an SNMP walk of the device */
	if ([walk.ip length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"IP Address Required"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"An IP address is required. This is the address of the device to be scanned."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:scanDeviceSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}	
	if ([walk.community length] < 1)
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"SNMP Community Required"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"An SNMP Community string is required. This is the plain-text shared key or password that is used to authenticate the SNMP requests."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:scanDeviceSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
	if (walk.replaceExistingScan) [[self document] removeAllObjectsFromOids];
	[walk startWalk];
	if (walk.sessionOpen)
	{
		[self setScanResult:nil];
		[self setScanInProgress:YES];
	}
	else
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Failed to open SNMP Session"
										 defaultButton:@"OK"
									   alternateButton:nil
										   otherButton:nil
							 informativeTextWithFormat:@"Please check the IP address and SNMP Community String. The device will need to be configured to accept the SNMP community string for read-only access.\n\nTry using the common default value of 'public'."];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:scanDeviceSheet
						  modalDelegate:self
						 didEndSelector:nil
							contextInfo:nil];
		return;
	}
}

- (IBAction) scanDeviceCancelClicked:(id)sender
{
	[walk cancel];
	[walk setDelegate:nil];
	self.walk = nil;
	[NSApp endSheet:scanDeviceSheet];
	[scanDeviceSheet close];
}

- (void) scanDidFinished:(BOOL)success error:(NSString *)error
{
	[self setScanInProgress:NO];
	[self setScanResult:error];
	[walk setDelegate:nil];

	if (success)
	{ 
		[self setScanDate:[NSDate date]]; 
		[NSApp endSheet:scanDeviceSheet];
		[scanDeviceSheet close];
	}
	
	[[self document] updateOIDSectionNames];
}

- (BOOL) scanInProgress
{ return scanInProgress; }

- (void) setScanInProgress:(BOOL)flag
{ scanInProgress = flag; }

- (NSDate *) scanDate
{ return scanDate; }

- (void) setScanDate:(NSDate *)date
{
	[scanDate release];
	scanDate = [date retain];
}

- (NSString *) scanAddress
{ return scanAddress; }

- (void) setScanAddress:(NSString *)string
{
	[scanAddress release];
	scanAddress = [string retain];
}

- (NSString *) scanCommunity
{ return scanCommunity; }

- (void) setScanCommunity:(NSString *)string
{ 
	[scanCommunity release];
	scanCommunity = [string retain];
}

- (NSString *) scanResult
{ return scanResult; }

- (void) setScanResult:(NSString *)string
{
	[scanResult release];
	scanResult = [string retain];
}

#pragma mark MIB Loading Methods

- (IBAction) loadMIBClicked:(id)sender
{
	/* Load a MIB */
	NSOpenPanel *panel = [[NSOpenPanel openPanel] retain];
	[panel setAllowsMultipleSelection:YES];
	[panel beginSheetForDirectory:nil
							 file:nil
				   modalForWindow:[self window]
					modalDelegate:self
				   didEndSelector:@selector(mibOpenSheetDidEnd:returnCode:contextInfo:)
					  contextInfo:nil];
}

- (void)mibOpenSheetDidEnd:(NSOpenPanel *)panel returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		NSEnumerator *pathEnum = [[panel filenames] objectEnumerator];
		NSString *path;
		while (path = [pathEnum nextObject])
		{
			MBMib *mib = [MBMib mibFromFile:path];
			[[self document] insertObject:mib inMibsAtIndex:[[self document] countOfMibs]];
		}
		
		/* Restart SNMP to re-load MIBs */
		restart_snmp ();
	}
	
	[panel release];
}

#pragma mark Container Methods

- (MBContainer *) selectedContainer
{ 
	NSArray *selectedObjects = [containerTreeController selectedObjects];
	if ([selectedObjects count] < 1) return nil;
	MBContainer *selectedContainer = (MBContainer *) [[selectedObjects objectAtIndex:0] container];
	return selectedContainer;
}

- (NSArray *) selectedContainers
{ 
	NSMutableArray *selection = [NSMutableArray array];
	for (MBEntity *entity in [containerTreeController selectedObjects])
	{
		if ([entity container] && ![selection containsObject:entity])
		{ [selection addObject:entity]; }
	}
	return selection;
}

- (IBAction) addNewContainerClicked:(id)sender
{
	MBContainer *container = [MBContainer container];
	[[self document] insertObject:container inContainersAtIndex:[[self document] countOfContainers]];
	[[self undoManager] setActionName:@"Add New Container"];
	[containerOutlineView reloadData];
	[containerOutlineView selectContainer:container];	
}

- (IBAction) removeSelectedContainersClicked:(id)sender
{
	
}

- (MBContainer *) activeContainer
{ return activeContainer; }

- (void) setActiveContainer:(MBContainer *)container
{
	[activeContainer release];
	activeContainer = [container retain];
}

#pragma mark "Metric Methods"

- (MBMetric *) selectedMetric
{ 
	int level = [containerOutlineView levelForRow:[containerOutlineView selectedRow]];
	MBMetric *selectedMetric = nil;
	MBTrigger *selectedTrigger = nil;
	NSArray *selectedObjects = [containerTreeController selectedObjects];
	if ([selectedObjects count] < 1) return nil;
	switch (level)
	{
		case 1:
			selectedMetric = [selectedObjects objectAtIndex:0];
			break;
		case 2:
			selectedTrigger = [selectedObjects objectAtIndex:0];
			selectedMetric = (MBMetric *) [selectedTrigger parent];
			break;
	}
			
	return selectedMetric;
}

- (NSArray *) selectedMetrics
{ 
	NSMutableArray *selection = [NSMutableArray array];
	for (MBEntity *entity in [containerTreeController selectedObjects])
	{
		if ([entity metric] && ![selection containsObject:entity])
		{ [selection addObject:entity]; }
	}
	return selection;
}

- (IBAction) metricPropertiesClicked:(id)sender
{
	[leftTabView selectTabViewItemAtIndex:1];
}

- (IBAction) newRateMetricClicked:(id)sender
{
	MBRateMetric *rateMetric = [MBRateMetric new];
	[[self selectedContainer] insertObject:rateMetric inChildrenAtIndex:[[[self selectedContainer] children] count]];	
	[containerOutlineView reloadData];
	[containerOutlineView selectMetric:rateMetric];
}

- (IBAction) newPercentMetricClicked:(id)sender
{
	MBPercentMetric *percentMetric = [MBPercentMetric new];
	[[self selectedContainer] insertObject:percentMetric inChildrenAtIndex:[[[self selectedContainer] children] count]];	
	[containerOutlineView reloadData];
	[containerOutlineView selectMetric:percentMetric];
}

- (IBAction) newDynamicMetricClicked:(id)sender
{
	[NSMenu popUpContextMenu:newDynamicMetricMenu withEvent:[NSApp currentEvent] forView:[[self window] contentView]];
}

#pragma mark "Trigger Methods"

- (MBTrigger *) selectedTrigger
{ 
	int level = [containerOutlineView levelForRow:[containerOutlineView selectedRow]];
	MBTrigger *selectedTrigger = nil;
	NSArray *selectedObjects = [containerTreeController selectedObjects];
	if ([selectedObjects count] < 1) return nil;
	switch (level)
	{
		case 2:
			selectedTrigger = [selectedObjects objectAtIndex:0];
			break;
	}
	
	return selectedTrigger;
}

- (void) newTriggerClicked:(id)sender
{
	if (![self selectedMetric]) return;

	[self newTriggerForMetric:[self selectedMetric]];	
}

- (void) newTriggerForMetric:(MBMetric *)metric
{
	self.newTriggerMetric = metric;
	self.newTrigger = [[MBTrigger new] autorelease];
	[self.newTrigger setParent:newTriggerMetric];
	
	/* Select the best-fit sevserity */
	int highestSeverity = 0;
	for (MBTrigger *trg in [newTriggerMetric children])
	{
		if ([[trg severity] intValue] > highestSeverity)
		{ highestSeverity = [[trg severity] intValue]; }
	}
	if (highestSeverity < 3 )
	{ 
		[self.newTrigger setSeverity:[NSNumber numberWithInt:highestSeverity+1]]; 
		[self.newTrigger setCondition:[NSNumber numberWithInt:5]];
	}
	else
	{ 
		[self.newTrigger setSeverity:[NSNumber numberWithInt:3]]; 
		[self.newTrigger setCondition:[NSNumber numberWithInt:3]];
	}

	[newTriggerSheet makeFirstResponder:newTriggerXValue];
	[NSApp beginSheet:newTriggerSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (void) newTriggerAddClicked:(id)sender
{
	/* Check for dup */
	for (MBTrigger *trg in [newTriggerMetric children])
	{
		if ([[trg desc] isEqualToString:[self.newTrigger desc]])
		{
			NSAlert *alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"OK"];
			[alert setMessageText:[NSString stringWithFormat:@"Duplicate %@ Description", [newTrigger typeString]]];
			[alert setInformativeText:[NSString stringWithFormat:@"Each %@ under a Metric must have a unique description.", [newTrigger typeString]]];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:newTriggerSheet 
							  modalDelegate:self
							 didEndSelector:@selector(duplicateAlertDidEnd:returnCode:contextInfo:) 
								contextInfo:nil];
			return;
		}
	}	
	
	/* Check if valid */
	if ([[newTrigger condition] intValue] == 5)
	{
		if ([[newTrigger xValue] floatValue] >= [[newTrigger yValue] floatValue])
		{
			NSAlert *alert = [[[NSAlert alloc] init] autorelease];
			[alert addButtonWithTitle:@"OK"];
			[alert setMessageText:[NSString stringWithFormat:@"Invalid Value Range", [newTrigger typeString]]];
			[alert setInformativeText:[NSString stringWithFormat:@"The value range entered is invalid.\nThe second value must be greater than the first."]];
			[alert setAlertStyle:NSWarningAlertStyle];
			[alert beginSheetModalForWindow:newTriggerSheet 
							  modalDelegate:self
							 didEndSelector:@selector(invalidAlertDidEnd:returnCode:contextInfo:) 
								contextInfo:nil];			
			return;
		}
	}
	
	/* Check for overlap */
	MBTrigger *overlapTrigger = [newTrigger checkForOverlap];
	if (overlapTrigger)
	{ 
		[self showOverlapAlertSheetFor:overlapTrigger]; 
		return;
	}
	
	
	/* Add */
	[newTriggerMetric insertObject:self.newTrigger inChildrenAtIndex:[[newTriggerMetric children] count]];
	[containerOutlineView reloadData];
	[containerOutlineView selectTrigger:newTrigger];
	[NSApp endSheet:newTriggerSheet];
	[newTriggerSheet close];
	self.newTrigger = nil;
	self.newTriggerMetric = nil;
}

- (void) newTriggerCancelClicked:(id)sender
{
	[NSApp endSheet:newTriggerSheet];
	[newTriggerSheet close];
	self.newTrigger = nil;
	self.newTriggerMetric = nil;
}

- (void) showOverlapAlertSheetFor:(MBTrigger *)overlapTrigger
{
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	NSButton *addAnywayButton = [alert addButtonWithTitle:@"Add Anyway"];
	[addAnywayButton setTag:2];
	[alert setMessageText:[NSString stringWithFormat:@"Trigger Overlaps with %@ Trigger", [overlapTrigger desc]]];
	[alert setInformativeText:[NSString stringWithFormat:@"The new trigger overlaps with the existing %@ trigger (%@).", [overlapTrigger desc], [overlapTrigger conditionMatchString]]];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:newTriggerSheet 
					  modalDelegate:self
					 didEndSelector:@selector(overlapAlertDidEnd:returnCode:contextInfo:) 
						contextInfo:nil];	
}

- (void) duplicateAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
}

- (void) overlapAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == 2)
	{
		/* Add Anyway clicked */
		[newTriggerMetric insertObject:self.newTrigger inChildrenAtIndex:[[newTriggerMetric children] count]];
		[containerOutlineView reloadData];
		[containerOutlineView selectTrigger:newTrigger];
		[NSApp endSheet:newTriggerSheet];
		[newTriggerSheet close];
		self.newTrigger = nil;
		self.newTriggerMetric = nil;
	}
}

- (void) invalidAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
}

@synthesize newTrigger;
@synthesize newTriggerMetric;

#pragma mark "Module Export"

- (IBAction) exportToXMLClicked:(id)sender
{
	/* Check if saved */
	if (![[self document] fileName])
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Document has not been saved. Please save the document before uploading to Lithium Core."
										 defaultButton:@"Save"
									   alternateButton:@"Cancel"
										   otherButton:nil
							 informativeTextWithFormat:@"The name of the Module uploaded will be taken from the filename of the Module Builder Document."];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert beginSheetModalForWindow:[self window]
						  modalDelegate:self
						 didEndSelector:@selector(exportUploadSaveAlertEnded:returnCode:contextInfo:)
							contextInfo:nil];
		
		return;
	}
		
	if ([[NSUserDefaults standardUserDefaults] objectForKey:@"coreDeployments"])
	{ 
		[[self document] setCoreDeployments:[NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"coreDeployments"]]]; 
		for (MBCoreDeployment *core in [[self document] coreDeployments])
		{
			if (core.ipAddress) [core refreshDeployment];
		}
	}
	else
	{ [[self document] setCoreDeployments:[NSMutableArray array]]; }
	[NSApp beginSheet:exportSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) exportSaveXMLToFileClicked:(id)sender
{
	/* Save the XML to a Local File */
	[self saveCoreDeploymentList];
	[NSApp endSheet:exportSheet];
	[exportSheet close];
	NSSavePanel *savePanel = [[NSSavePanel savePanel] retain];
	[savePanel setRequiredFileType:@"xml"];
	[savePanel beginSheetForDirectory:nil
								 file:[NSString stringWithFormat:@"%@.xml", [[[[[self document] fileName] lastPathComponent] stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:@" " withString:@""]]
					   modalForWindow:[self window]
						modalDelegate:self
					   didEndSelector:@selector(exportSaveXMLToFileSheetEnded:returnCode:contextInfo:)
						  contextInfo:nil];
}

- (void) exportSaveXMLToFileSheetEnded:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Save */
		[[self document] outputXmlToFile:[sheet filename]];
	}
	[sheet close];
	[sheet release];
}

- (IBAction) exportUploadClicked:(id)sender
{
	/* Upload to selected deployment */
	self.totalRequests = 0;
	self.completedRequests = 0;
	[self setExportUploadRequests:[NSMutableArray array]];
	NSXMLDocument *xml = [[self document] compileXml];
	for (MBCoreDeployment *core in [[self document] coreDeployments])
	{
		if (!core.selected) continue;
		NSString *name = [[[[[self document] fileName] lastPathComponent] stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:@" " withString:@""];
		MBModuleUploadRequest *req = [MBModuleUploadRequest uploadModule:[xml XMLData] named:name toCore:core];
		req.delegate = self;
		[req performAsyncRequest];
		[self insertObject:req inExportUploadRequestsAtIndex:[exportUploadRequests count]];
		self.totalRequests = self.totalRequests + 1;
	}
	
	/* Save Core Deployments */
	[self saveCoreDeploymentList];
	
	/* Switch to progress sheet */
	exportUploadInProgress = YES;
	[NSApp endSheet:exportSheet];
	[exportSheet close];
	[NSApp beginSheet:exportProgressSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (void) exportUploadSaveAlertEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Save it */
		[[alert window] orderOut:self];
		[[self document] saveDocumentWithDelegate:self didSaveSelector:@selector(document:didSaveForExport:contextInfo:) contextInfo:nil];
	}
}

- (void)document:(NSDocument *)doc didSaveForExport:(BOOL)didSave contextInfo:(void  *)contextInfo
{
	if (didSave)
	{ [self exportToXMLClicked:self]; }
}

- (void) moduleUploadFinished:(id)sender
{
	self.completedRequests = self.completedRequests + 1;
	if (self.completedRequests == self.totalRequests)
	{
		/* All Finished */
		exportUploadInProgress = NO;
	}
}

- (IBAction) exportCancelClicked:(id)sender
{
	/* Cancel */
	[self saveCoreDeploymentList];
	[NSApp endSheet:exportSheet];
	[exportSheet close];
}

- (IBAction) exportProgressCancelClicked:(id)sender
{
	/* Cancel uploads in progress */
	for (MBModuleUploadRequest *req in exportUploadRequests)
	{
		[req cancel];
	}
	[self setExportUploadRequests:nil];
	[NSApp endSheet:exportProgressSheet];
	[exportProgressSheet close];
}

- (IBAction) exportProgressCloseClicked:(id)sender
{
	/* Close window */
	[self setExportUploadRequests:nil];	
	[NSApp endSheet:exportProgressSheet];
	[exportProgressSheet close];
}

- (IBAction) exportAddDeploymentClicked:(id)sender
{
	MBCoreDeployment *core = [MBCoreDeployment new];
	[[self document] insertObject:core inCoreDeploymentsAtIndex:[[[self document] coreDeployments] count]];
}

- (IBAction) exportRemoveDeploymentClicked:(id)sender
{
	for (MBCoreDeployment *core in [coreDeploymentArrayController selectedObjects])
	{
		[[self document] removeObjectFromCoreDeploymentsAtIndex:[[[self document] coreDeployments] indexOfObject:core]];
	}
}

- (IBAction) exportRefreshDeploymentClicked:(id)sender
{
	for (MBCoreDeployment *core in [coreDeploymentArrayController selectedObjects])
	{
		[core refreshDeployment];
	}
}

#pragma mark "Core Deployment List"

- (void) saveCoreDeploymentList
{
	/* Save Core Deployments */
	if ([[self document] coreDeployments])
	{ 
		[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:[[self document] coreDeployments]]
												  forKey:@"coreDeployments"]; 
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
}

#pragma mark "Feedback"

- (IBAction) sendFeedbackClicked:(id)sender
{
	[[LCFeedbackController alloc] initForWindow:[self window]];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem
{
    SEL theAction = [anItem action];
	
	/*
	 * Toolbars 
	 */
	
    if (theAction == @selector(loadMIBClicked:))
    { return YES; }
	else if (theAction == @selector(scanDeviceClicked:))
	{ return YES; }
	else if (theAction == @selector(addNewContainerClicked:))
	{ return YES; }
	else if (theAction == @selector(newTriggerClicked:))
	{ 
		if ([self selectedMetric] && [[self window] firstResponder] == containerOutlineView) return YES;
		else return NO;
	}
	else if (theAction == @selector(newDynamicMetricClicked:))
	{ 
		if ([self selectedMetric] && [[self window] firstResponder] == containerOutlineView) return YES;
		else return NO;
	}
	else if (theAction == @selector(newRateMetricClicked:))
	{ 
		if ([self selectedMetric] && [[self window] firstResponder] == containerOutlineView) return YES;
		else return NO;
	}
	else if (theAction == @selector(newPercentMetricClicked:))
	{ 
		if ([self selectedMetric] && [[self window] firstResponder] == containerOutlineView) return YES;
		else return NO;
	}
	else if (theAction == @selector(exportToXMLClicked:))
	{ return YES; }
	
	/*
	 * Menu Bar Items 
	 */
	
	else if (theAction == @selector(exportToXMLClicked:))
	{ return YES; }
	else if (theAction == @selector(exportSaveXMLToFileClicked:))
	{ return YES; }
	else if (theAction == @selector(delete:) && [self selectedContainer])
	{ return YES; }
	else if (theAction == @selector(delete:) && [self selectedMetric])
	{ return YES; }
	else if (theAction == @selector(delete:) && [self selectedTrigger])
	{ return YES; }
	
	return NO;
}		

#pragma mark "Properties"

@synthesize savePasswordToKeyChain;
@synthesize exportUploadRequests;
- (void) setExportUploadRequests:(NSMutableArray *)value
{
	[exportUploadRequests release];
	exportUploadRequests = [value mutableCopy];
}
- (void) insertObject:(MBModuleUploadRequest *)obj inExportUploadRequestsAtIndex:(unsigned int)index
{
	[exportUploadRequests insertObject:obj atIndex:index];
}
- (void) removeObjectFromExportUploadRequestsAtIndex:(unsigned int)index
{
	[exportUploadRequests removeObjectAtIndex:index];
}

@synthesize completedRequests;
@synthesize totalRequests;

#pragma mark Accessors

- (void)setDocument:(NSDocument *)document
{
	[super setDocument:document];
	[containerTreeController setDocument:[self document]];
}

@synthesize exportUploadInProgress;

#pragma mark "Search"
@synthesize searchTabTag;

@synthesize searchString;
- (void) setSearchString:(NSString *)value
{
	[searchString release];
	searchString = [value copy];
	if (searchString && [searchString length] > 0)
	{
		/* Search string present */
		NSPredicate *searchPredicate = [NSPredicate predicateWithFormat:@"(name contains[cd] %@) OR (oid contains[cd] %@) OR (value contains[cd] %@)", 
										searchString, searchString, searchString];
		self.searchResults = [[[self document] oids] filteredArrayUsingPredicate:searchPredicate];
		self.searchTabTag = @"search";
	}
	else
	{
		/* Search Ended */
		self.searchTabTag = @"browse";
		self.searchResults = nil;
	}
}

@synthesize searchResults;


@end
