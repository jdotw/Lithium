//
//  LCActionListWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCActionListWindowController.h"
#import "LCAction.h"
#import "LCActionEditWindowController.h"
#import "LCActionScriptManagerController.h"
#import "LCLithiumSetupWindowController.h"

@implementation LCActionListWindowController

#pragma mark "Constructors"

- (LCActionListWindowController *) initForCustomer:(LCCustomer *)initCustomer
{
	/* Load the NIB */
	[super initWithWindowNibName:@"ActionListWindow"];
	[self window];
	[backgroundView setImage:[NSImage imageNamed:@"slateback.png"]];	
	[self buildToolbar];

	/* Set customer */
	self.customer = initCustomer;
	
	/* Create List */
	self.actionList = [LCActionList actionListForCustomer:initCustomer];
	self.actionList.customer = self.customer;
	[actionList highPriorityRefresh];
	
	/* Show */
	[[self window] makeKeyAndOrderFront:self];
	
	return self;
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[customer release];
	[actionList release];
	[super dealloc];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"ActionList"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Refresh list */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Refresh"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (refreshListClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Refresh.tiff"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Add action */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Add Action"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (addNewActionClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"add_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Remove action */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Delete Action"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (deleteActionClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"trash_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			
	
	/* Edit action */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Edit Action"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (editActionClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"edit_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Spacer */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Refresh Selector */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"Refresh Indicator"] autorelease];
	[tbitem setLabel:@""];
	[tbitem setTarget: self];
	[tbitem setView:refreshView];
	[tbitem setMinSize:[refreshView bounds].size];
	[tbitem setMaxSize:[refreshView bounds].size];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Edit action */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Manage Scripts"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (manageScriptsClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"tools_script_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
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

#pragma mark "UI Actions"

- (IBAction) addNewActionClicked:(id)sender
{
	/* Add new */
	LCActionEditWindowController *controller = [[LCActionEditWindowController alloc] initForNewAction:customer];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (IBAction) deleteActionClicked:(id)sender
{
	/* Check selection */
	LCAction *action = [self selectedAction];
	if (!action) return;
	
	/* Open sheet */
	[NSApp beginSheet:deleteSheet 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) editActionClicked:(id)sender
{
	LCAction *action = [self selectedAction];
	if (!action) return;
	
	LCAction *dupAction = [[action copy] autorelease];
	LCActionEditWindowController *controller = [[LCActionEditWindowController alloc] initForAction:dupAction];
	[NSApp beginSheet:[controller window] 
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) refreshListClicked:(id)sender
{
	[actionList highPriorityRefresh];
	[[[self selectedAction] historyList] highPriorityRefresh];
}

- (IBAction) deleteSheetDeleteClicked:(id)sender
{
	/* Delete action */
	LCAction *action = [self selectedAction];
	if (action)
	{
		[action performXmlDelete];
		[actionList removeObjectFromObjectsAtIndex:[[actionList objects] indexOfObject:action]];
	}

	/* Close sheet */
	[NSApp endSheet:deleteSheet];
	[deleteSheet close];
}

- (IBAction) deleteSheetCancelClicked:(id)sender
{
	/* Do NOT Delete */
	[NSApp endSheet:deleteSheet];
	[deleteSheet close];
}

- (IBAction) manageScriptsClicked:(id)sender
{
	[[LCLithiumSetupWindowController alloc] initForCustomer:customer showingTab:@"actionscripts"];
}

#pragma mark "Selection Management"

- (LCAction *) selectedAction
{
	if ([[actionArrayController selectedObjects] count] < 1) return nil;
	else return [[actionArrayController selectedObjects] objectAtIndex:0];
}

#pragma mark "Accessors" 

@synthesize customer;
@synthesize actionList;

@end
