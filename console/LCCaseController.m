#import "LCCaseController.h"
#import "LCXMLRequest.h"
#import "LCCustomerList.h"
#import "LCEntity.h"
#import "LCMetricGraphDocument.h"
#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"

@implementation LCCaseController

#pragma mark "Initialisation"

- (LCCaseController *) initWithCase:(LCCase *)initcase
{
	/* Set case */
	[self setCas:initcase];

	/* Init */
	[self init];
		
	/* Set auto-refresh */
	[self setLogEntryListAutoRefresh:YES];
	[[[self cas] logEntryList] highPriorityRefresh];
	[self setEntityListAutoRefresh:YES];
	[[[self cas] entityList] highPriorityRefresh];
	
	return self;
}

- (LCCaseController *) initForNewCaseAtCustomer:(LCCustomer *)initCustomer
{
	self = [self initForNewCase];
	
	self.openCustomerString = [initCustomer name];
	[openCustomer setEnabled:NO];

	return self;
}

- (LCCaseController *) initForNewCase
{
	/* Create blank case */
	self.cas = [LCCase new];

	/* Init */
	[self init];
	
	/* Open new case sheet */
	[NSApp beginSheet:openWindow
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	[[self window] setTitle:@"New Case"];
	[logTableView setDelegate:self];

	/* Customer list */
	for (LCCustomer *aCustomer in [[LCCustomerList masterList] array])
	{ [openCustomer addItemWithObjectValue:aCustomer.name]; }
	
	/* Entity list */
	openEntityArray = [[NSMutableArray array] retain];
	
	return self;
}

- (LCCaseController *) initForNewCaseWithEntityList:(NSArray *)entarray
{
	/* Init */
	[self initForNewCase];

	/* Populate entity list */
	for (LCEntity *entity in entarray)
	{ [self insertObject:entity inOpenEntityArrayAtIndex:[openEntityArray count]]; }
	
	/* Set customer field */
	if ([entarray count] > 0)
	{ 
		self.openCustomerString = [[[entarray objectAtIndex:0] customer] name];
		[openCustomer setEnabled:NO];
	}
	
	return self;
}

- (LCCaseController *) init 
{
	[super init];

	/* ivar init */
	logOperationCloseCase = NO;

	/* Create rowHeightArray */
	rowHeightArray = [[NSMutableArray array] retain];
	
	/* Load/setup nib */
	[super initWithWindowNibName:@"CaseWindow"];
	[self window];
	[logTableView sizeToFit];
	[self buildToolbar];
	[backgroundView setImage:[NSImage imageNamed:@"slateback.png"]];
	[contentView setBackImage:[NSImage imageNamed:@"browser_objback.png"]];
	[openEntitiesController setCas:[self cas]];
	[openEntitiesController setAllowDrop:YES];
	[entitiesController setCas:[self cas]];
	[entitiesController setAllowDrop:YES];
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[cas release];
	[openEntityArray release];
	[toolbar release];
	[toolbarItems release];
	[toolbarSelectableItems release];
	[toolbarDefaultItems release];
	[caseRefreshTimer invalidate];
	[logEntryListRefreshTimer invalidate];
	[entityListRefreshTimer invalidate];
	[rowHeightArray release];
	
	[super dealloc];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	/* Obtain selection */
	SEL action = [item action];
	
	if (action == @selector(closeCaseClicked:))
	{ if (![[cas customer] userIsNormal]) return NO; }

	if (action == @selector(updateLogClicked:))
	{ if (![[cas customer] userIsNormal]) return NO; }

	if (action == @selector(changeHeadlineClicked:))
	{ if (![[cas customer] userIsNormal]) return NO; }

	if (action == @selector(reassignClicked:))
	{ if (![[cas customer] userIsNormal]) return NO; }
	
	return YES;
}

#pragma mark "Case opening"

- (IBAction) openOpenClicked:(id)sender
{
	LCCustomer *cust;
	
	/* Find cust */
	cust = [[[LCCustomerList masterList] dict] objectForKey:openCustomerString];
	if (!cust)
	{
		/* Customer not found */
		[openInfoField setStringValue:@"Error: Invalid customer entered"];
		[openInfoField setHidden:NO];
		return;
	}

	/* Set progress */
	[openProgressBar startAnimation:self];
	[openInfoField setStringValue:@"Opening case..."];
	[openInfoField setHidden:NO];
	
	/* Disable buttons */
	[openOKButton setEnabled:NO];
	[openCancelButton setEnabled:NO];
	
	/* Create case */
	for (LCEntity *entity in openEntityArray)
	{ [cas.entityList insertObject:entity inEntitiesAtIndex:[cas.entityList.entities count]]; }
	cas.customer = cust;
	cas.headline = [openHeadline stringValue];
	cas.requester = [openRequester stringValue];
	cas.owner = [[LCAuthenticator authForCustomer:cust] username];	
	
	/* Open case */
	[cas openCaseWithInitialLogEntry:[openLogEntry string] delegate:self];
}

- (IBAction) openCancelClicked:(id)sender
{
	/* Cancel opening of new case */
	[NSApp endSheet:openWindow];
	[openWindow close];
	[[self window] close];
}

- (void) caseOpenFinished:(LCCase *)sender;
{
	/* Called when the case has been opened */

	/* Stop animation */
	[openProgressBar stopAnimation:self];
	[openInfoField setHidden:YES];
	
	/* Re-enable buttons */
	[openOKButton setEnabled:YES];
	[openCancelButton setEnabled:YES];
	
	/* Check result */
	if (!cas.caseID)
	{
		[openInfoField setStringValue:@"Error: Failed to open case"];
		[openInfoField setHidden:NO];
		return;
	}
	
	/* Close case opening sheet */
	[NSApp endSheet:openWindow];
	[openWindow close];	

	/* Set auto-refresh */
	self.logEntryListAutoRefresh = YES;
	[self.logEntryListRefreshTimer fire];
	self.entityListAutoRefresh = YES;
	[self.entityListRefreshTimer fire];	
}

#pragma mark "Case closing"

-(IBAction) closeCaseClicked:(id)sender
{
	/* Set case closing flag */
	logOperationCloseCase = YES;
	
	/* Setup & begin log sheet */
	[logEntry setString:@""];
	[logRecordButton setStringValue:@"Record and Close"];
	[NSApp beginSheet: logWindow
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (void) caseClosedFinished:(LCCase *)sender
{
	/* End log sheet */
	[self endLogEntrySheet];
	
	/* Close window */
	[[self window] close];
}

#pragma mark "Entities"

- (IBAction) graphSelectedEntityClicked:(id)sender
{
	for (LCEntity *entity in [entitiesController selectedObjects])
	{
		LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
		document.initialEntity = [entity metric];
		[[NSDocumentController sharedDocumentController] addDocument:document];
		[document makeWindowControllers];
		[document showWindows];
	}
}

- (IBAction) browseToSelectedEntityClicked:(id)sender
{
	for (LCEntity *entity in [entitiesController selectedObjects])
	{ [[[LCBrowser2Controller alloc] initWithEntity:entity] autorelease]; }
}

- (IBAction) faultHistoryForSelectedEntityClicked:(id)sender
{
	for (LCEntity *entity in [entitiesController selectedObjects])
	{ [[LCFaultHistoryController alloc] initForEntity:entity]; }
}

- (IBAction) metricHistoryForSelectedEntityClicked:(id)sender
{
	for (LCEntity *entity in [entitiesController selectedObjects])
	{ if ([entity metric]) [[LCMetricHistoryWindowController alloc] initWithMetric:[entity metric]]; }
}

- (IBAction) triggerTuningForSelectedEntityClicked:(id)sender
{
	for (LCEntity *entity in [entitiesController selectedObjects])
	{ if ([entity object]) [[LCTriggerTuningWindowController alloc] initWithObject:[entity object]]; }		
}

#pragma mark "Log Entries"

-(IBAction) updateLogClicked:(id)sender
{
	/* Setup & begin log sheet */
	[logEntry setString:@""];
	[NSApp beginSheet: logWindow
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

-(IBAction) logRecordClicked:(id)sender
{
	/* Check if case is closing, or if this is just a log update */
	if (logOperationCloseCase == YES)
	{
		/* Close case */
		[cas closeCaseWithFinalLogEntry:[logEntry string] delegate:self];
		[logInfoField setStringValue:@"Closing case..."];
	}
	else
	{
		/* Record log entry */
		LCCaseLogEntry *log = [[LCCaseLogEntry alloc] initWithString:[logEntry string] cas:cas timespent:[[logTimeSpent stringValue] floatValue]];
		log.delegate = self;
		[log record:cas];
		[logInfoField setStringValue:@"Recording log entry..."];
		[log autorelease];
	}
	
	/* Set progress */
	[logRecordButton setEnabled:NO];
	[logCancelButton setEnabled:NO];
	[logProgressBar startAnimation:self];
	[logInfoField setHidden:NO];
}


- (void) logEntryRecordFinished:(LCCaseLogEntry *)sender
{
	/* End log sheet */
	[self endLogEntrySheet];
}

-(IBAction) logCancelClicked:(id)sender
{
	[self endLogEntrySheet];
}

-(void) endLogEntrySheet
{
	[logRecordButton setEnabled:YES];
	[logCancelButton setEnabled:YES];
	[logProgressBar stopAnimation:self];
	[logInfoField setHidden:YES];
	[NSApp endSheet:logWindow];	
	[logWindow close];
}

- (NSString *) logEntryDisplayString:(LCCaseLogEntry *)log
{
	return [NSString stringWithFormat:@"%@ - %@\n\n%@",	[log.timestamp description], log.author, log.entry];	
}

#pragma mark "TableView Methods"

- (float)tableView:(NSTableView *)tableView heightOfRow:(int)row
{
	/* Calculate row height */
	NSTableColumn *col = [[logTableView tableColumns] objectAtIndex:0];
	float width = [col width];
	NSRect rect = NSMakeRect(0, 0, width, 5000.0);
	NSCell *cell = [col dataCellForRow:row]; 
	[cell setWraps:YES];
	LCCaseLogEntry *log = [[[cas logEntryList] logEntries] objectAtIndex:row];
	NSString *content = [log displayString];
	[cell setObjectValue:content];
	float height = [cell cellSizeForBounds:rect].height + 5;
	if (height <= 0) height = 16.0;
	return height;
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"CaseToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: YES];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];

	/* Update case item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Update"] autorelease];
	[tbitem setLabel:@"Update Log"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (updateLogClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"UpdateLog.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Update"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Close case item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Close Case"] autorelease];
	[tbitem setLabel:@"Close Case"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (closeCaseClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseClose.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Close Case"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Refresh Entities */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Entities"] autorelease];
	[tbitem setLabel:@"Refresh Entities"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshEntitiesClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshEntity.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Entities"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Refresh Log Entries */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh Log Entries"] autorelease];
	[tbitem setLabel:@"Refresh Log Entries"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshLogEntriesClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"RefreshNotes.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh Log Entries"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Re-Assign */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Re-Assign Case"] autorelease];
	[tbitem setLabel:@"Re-Assign Case"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (reassignClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"user_48.tif"]];
	[toolbarItems setObject:tbitem forKey:@"Re-Assign Case"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Change Headline */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Change Headline"] autorelease];
	[tbitem setLabel:@"Change Headline"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (changeHeadlineClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Info.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Change Headline"];
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

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove bindings/observers */
	[controllerAlias setContent:nil];
	
	/* Disable auto-refresh */
	[self setEntityListAutoRefresh:NO];
	[self setLogEntryListAutoRefresh:NO];
	
	/* Autorelease */
	[self autorelease];
}

#pragma mark "Refresh Routines"

@synthesize logEntryListAutoRefresh;
- (void) setLogEntryListAutoRefresh:(BOOL)flag
{
	if (flag == YES && !logEntryListRefreshTimer)
	{
		/* Add timer */
		logEntryListRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
																	target:[cas logEntryList]
																  selector:@selector(normalPriorityRefresh)
																  userInfo:nil 
																   repeats:YES];
		
	}
	if (flag == NO && logEntryListRefreshTimer)
	{
		/* Remove Timer */
		[logEntryListRefreshTimer invalidate];
		logEntryListRefreshTimer = nil;
	}
}
@synthesize logEntryListRefreshTimer;

@synthesize entityListAutoRefresh;
- (void) setEntityListAutoRefresh:(BOOL)flag
{
	if (flag == YES && !entityListRefreshTimer)
	{
		/* Add timer */
		entityListRefreshTimer = [NSTimer scheduledTimerWithTimeInterval:60.0
																  target:[cas entityList]
																selector:@selector(normalPriorityRefresh)
																  userInfo:nil 
																 repeats:YES];
		
	}
	if (flag == NO && entityListRefreshTimer)
	{
		/* Remove Timer */
		[entityListRefreshTimer invalidate];
		entityListRefreshTimer = nil;
	}
}
@synthesize entityListRefreshTimer;

- (IBAction) refreshEntitiesClicked:(id)sender
{
	[[cas entityList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

- (IBAction) refreshLogEntriesClicked:(id)sender
{
	[[cas logEntryList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "Case Assignment Methods"

- (IBAction) reassignClicked:(id)sender
{
	/* Clear field */
	[reassignTextField setStringValue:@""];
	[reassignSheet makeFirstResponder:reassignTextField];
	
	/* Start sheet */
	[NSApp beginSheet:reassignSheet 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];
}

- (IBAction) reassignSheetAssignClicked:(id)sender
{
	/* Update case */
	[cas setOwner:[reassignTextField stringValue]];
	[cas updateCase];
	
	/* End Sheet */
	[NSApp endSheet:reassignSheet];
	[reassignSheet close];	
}

- (IBAction) reassignSheetCancelClicked:(id)sender
{
	/* End Sheet */
	[NSApp endSheet:reassignSheet];
	[reassignSheet close];
}

#pragma mark "Headline Change Methods"

- (IBAction) changeHeadlineClicked:(id)sender
{
	/* Clear field */
	[changeHeadlineTextField setStringValue:@""];
	[changeHeadlineSheet makeFirstResponder:changeHeadlineTextField];
	
	/* Start sheet */
	[NSApp beginSheet:changeHeadlineSheet 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) changeHeadlineSaveClicked:(id)sender
{
	/* Update case */
	[cas setHeadline:[changeHeadlineTextField stringValue]];
	[cas updateCase];
	
	/* End Sheet */
	[NSApp endSheet:changeHeadlineSheet];
	[changeHeadlineSheet close];		
}

- (IBAction) changeHeadlineCancelClicked:(id)sender
{
	/* End Sheet */
	[NSApp endSheet:changeHeadlineSheet];
	[changeHeadlineSheet close];
}

#pragma mark "Case Re-opening Methods"

- (IBAction) reOpenCaseClicked:(id)sender
{
	/* Check state */
	if (!cas.isClosed)
	{ return; }
	
	/* Clear field */
	[reOpenTextView setString:@""];
	[reOpenSheet makeFirstResponder:reOpenTextView];
	
	/* Start sheet */
	[NSApp beginSheet:reOpenSheet 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) reOpenCaseOpenClicked:(id)sender
{
	/* Re-open case */
	[cas reOpenCaseWithLogEntry:[reOpenTextView string] delegate:self];

	/* Set progress */
	[reOpenProgressBar startAnimation:self];
	[reOpenInfoField setStringValue:@"Re-Opening case..."];	
}

- (IBAction) reOpenCaseFinished:(id)sender
{
	/* Set progress */
	[reOpenProgressBar stopAnimation:self];
	[reOpenInfoField setStringValue:@""];
	
	/* End Sheet */
	[NSApp endSheet:reOpenSheet];
	[reOpenSheet close];
}

- (IBAction) reOpenCaseCancelClicked:(id)sender
{
	/* End Sheet */
	[NSApp endSheet:reOpenSheet];
	[reOpenSheet close];	
}

#pragma mark "Navigation/Display Methods"

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	NSArray *selectedEntities;
	
	/* Get selected entities */
	selectedEntities = [entitiesController selectedObjects];
	if (!selectedEntities || [selectedEntities count] < 1)
	{
		/* No entity is selected, use the entire list */
		selectedEntities = [entitiesController arrangedObjects];
	}

	/* Open graph window */
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
	NSArray *selectedEntities;
	
	/* Get selected entities */
	selectedEntities = [entitiesController selectedObjects];
	if (!selectedEntities || [selectedEntities count] < 1)
	{
		/* No entity is selected, use the entire list */
		selectedEntities = [entitiesController arrangedObjects];
	}

	if (selectedEntities && [selectedEntities count] > 0)
	{ [[[LCBrowser2Controller alloc] initWithEntity:[selectedEntities objectAtIndex:0]] autorelease]; }
}

#pragma mark "General Accessor Methods"

@synthesize cas;
@synthesize openEntityArray;
- (void) insertObject:(LCEntity *)entity inOpenEntityArrayAtIndex:(unsigned int)index
{ 
	/* Insert into array */
	[openEntityArray insertObject:entity atIndex:index];
	
	/* Check/Set customer */
	if (![cas customer])
	{
		LCCustomer *cust = [entity customer];
		if (cust)
		{ 
			[cas setCustomer:cust]; 
			[self setValue:[cust name] forKey:@"openCustomerString"];
			[openCustomer setEnabled:NO];
		}
	}
}
- (void) removeObjectFromOpenEntityArrayAtIndex:(unsigned int)index
{
	/* Remove from array */
	[openEntityArray removeObjectAtIndex:index];
	
	/* Check/Set customer */
	if ([openEntityArray count] == 0)
	{
		[cas setCustomer:nil];
		[self setValue:nil forKey:@"openCustomerString"];
		[openCustomer setEnabled:YES];
	}
}
@synthesize openCustomerString;
@synthesize openRequesterString;
@synthesize openHeadlineString;

@end
