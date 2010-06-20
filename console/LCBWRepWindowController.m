//
//  LCBWRepWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepWindowController.h"
#import "LCBWRepGroup.h"
#import "LCBWRepDocument.h"
#import "LCBrowser2Controller.h"
#import "LCMetricGraphDocument.h"
#import "LCCaseController.h"

@implementation LCBWRepWindowController

#pragma mark "Constructors"

- (LCBWRepWindowController *) initWithReportDocument:(id)initDocument
{
	/* Setup document */
	reportDocument = initDocument;
	
	/* Super-class init */
	[super initWithWindowNibName:@"BandwidthReport"];
	[self window];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	
	/* Setup/Show window */
	[treeController setReportDocument:reportDocument];
	[self buildToolbar];
	[[self window] makeKeyAndOrderFront:self];
	
	/* Check if refresh sheet should be shown */
	if ([reportDocument countOfHistoryRefreshList] > 0)
	{
		[self showRefreshSheet];
	}
	
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
	[controllerAlias setContent:nil];
	[self autorelease];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"BWReportToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Refresh item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh All"] autorelease];
	[tbitem setLabel:@"Refresh All"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Refresh All"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* New Group Item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"New Group"] autorelease];
	[tbitem setLabel:@"New Group"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (newGroupClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"CaseOpen.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"New Group"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* View Mode Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"View"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:viewModeView];
	[tbitem setMinSize:[viewModeView bounds].size];
	[tbitem setMaxSize:[viewModeView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Stats Mode Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Statistics"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:statsModeView];
	[tbitem setMinSize:[statsModeView bounds].size];
	[tbitem setMaxSize:[statsModeView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Discard Mode Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Missing Data"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:discardModeView];
	[tbitem setMinSize:[discardModeView bounds].size];
	[tbitem setMaxSize:[discardModeView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Reference Date Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Reference Date"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:dateView];
	[tbitem setMinSize:[dateView bounds].size];
	[tbitem setMaxSize:[dateView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			

	/* Reference Period Selection */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Reference Period"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setView:periodView];
	[tbitem setMinSize:[periodView bounds].size];
	[tbitem setMaxSize:[periodView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Export item */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Export Data"] autorelease];
	[tbitem setLabel:@"Export Data"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (exportClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Export.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Export Data"];
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

#pragma mark "Toolbar Action Methods"

- (IBAction) newGroupClicked:(id)sender
{
	/* Create a new group */
	LCBWRepGroup *group = [LCBWRepGroup group];
	
	/* Set name */
	int i;
	NSString *groupDesc = @"New Group";
	for (i=1; [reportDocument locateGroupItem:groupDesc]; i++)
	{ groupDesc = [NSString stringWithFormat:@"New Group %i", i]; }
	[group setDisplayDescription:groupDesc];
	
	/* Add to documents group dictionary */
 	[(LCBWRepDocument *)reportDocument addGroupItem:group];
	
	/* Add to appropriate parent */
	if ([[treeController selectedObjects] count] > 0)
	{
		LCBWRepItem *selectedItem = [[treeController selectedObjects] objectAtIndex:0];
		LCBWRepItem *parent;
		if ([selectedItem type] == BWREP_ITEM_GROUP)
		{
			/* Add to group */
			parent = selectedItem;
		}
		else
		{
			/* Add to group parent of selected item */
			parent = [selectedItem parentGroup];
		}
		
		if (parent)
		{
			/* Add to parent item */
			[parent insertObject:group inChildrenAtIndex:[parent countOfChildren]];
		}
		else
		{
			/* Add to document root */
			[(LCBWRepDocument *)reportDocument insertObject:group inItemsAtIndex:[reportDocument countOfItems]];
		}
	}
	else
	{
		/* Nothing selected, add it to the document root */
		[(LCBWRepDocument *)reportDocument insertObject:group inItemsAtIndex:[reportDocument countOfItems]];
	}
}

- (IBAction) refreshClicked:(id)sender
{
	[reportDocument refresh:XMLREQ_PRIO_HIGH];
}

- (IBAction) exportClicked:(id)sender
{
	NSSavePanel *sp;
	int runResult;
	
	/* create or get the shared instance of NSSavePanel */
	sp = [NSSavePanel savePanel];
	
	/* set up new attributes */
	[sp setTitle:@"Export Bandwidth Report"];
	[sp setRequiredFileType:@"csv"];
	[sp setCanCreateDirectories:YES];
	[sp setCanSelectHiddenExtension:YES];
	
	/* display the NSSavePanel */
	runResult = [sp runModalForDirectory:nil file:@"report.csv"];
	
	/* if successful, save file under designated name */
	if (runResult == NSOKButton) 
	{
		[reportDocument exportToCsv:[sp filename]];
	}	
}

#pragma mark "Document Accessor"

- (id) reportDocument
{ return reportDocument; }

#pragma mark "Selection Methods"

- (NSArray *) selectedObjects
{
	if ([reportDocument viewMode] == 0)
	{
		/* Tree view */
		return [treeController selectedObjects];
	}
	else if ([reportDocument viewMode] == 1)
	{
		/* List view */
		return [arrayController selectedObjects];
	}
	else
	{ return nil; }
}

- (id) firstSelectedObject
{
	if ([[self selectedObjects] count] > 0)
	{ return [[self selectedObjects] objectAtIndex:0]; }
	else
	{ return nil; }
}

#pragma mark "Group Menu Methods"

- (IBAction) arrangeByDeviceClicked:(id)sender
{
	LCBWRepGroup *selectedGroup = [self firstSelectedObject];
	if ([selectedGroup arrangeByDevice])
	{ [selectedGroup setArrangeByDevice:NO]; }
	else
	{ [selectedGroup setArrangeByDevice:YES]; }
}

- (IBAction) removeGroupClicked:(id)sender
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm group removal"
									 defaultButton:@"Remove" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"Click 'Remove' to confirm group removal"];
	int button = [alert runModal];
	if (button == NSAlertDefaultReturn)
	{
		NSEnumerator *groupEnum = [[self selectedObjects] objectEnumerator];
		LCBWRepGroup *group;
		while (group=[groupEnum nextObject])
		{ [group removeGroup]; }
	}		
}

#pragma mark "Common Menu Methods"

- (IBAction) browseToClicked:(id)sender
{
	NSEnumerator *itemEnum = [[self selectedObjects] objectEnumerator];
	LCBWRepItem *item;
	while (item=[itemEnum nextObject])
	{
		[[[LCBrowser2Controller alloc] initWithEntity:[item entity]] autorelease];
	}
}

- (IBAction) openCaseClicked:(id)sender
{
	NSArray *selectedEntities = [[self selectedObjects] valueForKey:@"entity"];
	[[LCCaseController alloc] initForNewCaseWithEntityList:selectedEntities];
}

#pragma mark "Device Menu Methods"

- (IBAction) removeDeviceClicked:(id)sender
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm device removal"
									 defaultButton:@"Remove" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"Click 'Remove' to confirm device removal"];
	int button = [alert runModal];
	if (button == NSAlertDefaultReturn)
	{
		NSEnumerator *deviceEnum = [[self selectedObjects] objectEnumerator];
		LCBWRepDevice *dev;
		while (dev=[deviceEnum nextObject])
		{ [dev removeDevice]; }
	}		
}

#pragma mark "Interface Menu Methods"

- (IBAction) graphSelectedClicked:(id)sender
{
	NSArray *selectedEntities = [[self selectedObjects] valueForKey:@"entity"];
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = selectedEntities;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (IBAction) removeInterfaceClicked:(id)sender
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm interface removal"
									 defaultButton:@"Remove" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"Click 'Remove' to confirm interface removal"];
	int button = [alert runModal];
	if (button == NSAlertDefaultReturn)
	{
		NSEnumerator *ifaceEnum = [[self selectedObjects] objectEnumerator];
		LCBWRepInterface *iface;
		while (iface=[ifaceEnum nextObject])
		{ [[iface parent] removeInterface:iface]; }
	}	
}

#pragma mark "Refresh Methods"

- (void) showRefreshSheet
{
	/* Open sheet */
	[NSApp beginSheet:refreshProgressSheet
	   modalForWindow:[self window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
	[self setRefreshSheetShown:YES];
}

- (void) hideRefreshSheet 
{
	[NSApp endSheet:refreshProgressSheet];
	[refreshProgressSheet close];
	[self setRefreshSheetShown:NO];
}

- (BOOL) refreshSheetShown
{ return refreshSheetShown; }

- (void) setRefreshSheetShown:(BOOL)value
{ refreshSheetShown = value; }

- (IBAction) cancelRefreshClicked:(id)sender
{
	[reportDocument cancelRefresh];
}

@synthesize reportDocument;
@synthesize controllerAlias;
@synthesize treeController;
@synthesize arrayController;
@synthesize statsModeView;
@synthesize viewModeView;
@synthesize discardModeView;
@synthesize dateView;
@synthesize periodView;
@synthesize backView;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@synthesize refreshProgressSheet;
@end
