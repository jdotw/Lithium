//
//  LCAdminController.m
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCAdminController.h"


@implementation LCAdminController

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	[self buildToolbar];
	
	[self statusClicked:self];

	[window makeKeyAndOrderFront:self];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"PreferencesToolbar"];
	[toolbar setAutosavesConfiguration: NO];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Status */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Status"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (statusClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"ok_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			
	
	/* Configure */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Configure"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (configureClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"configure_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Customers */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Customers"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction:@selector (customersClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"user_group_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Licenses */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Licenses"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (licensesClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"mortgage_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Custom Modules */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Modules"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction:@selector (modulesClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"modb48.png"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Update */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Update    "] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (updateClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"down_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Diagnostic */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Diagnostic"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget:self];
	[tbitem setAction:@selector (diagnosticClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"task_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];			


	/* Backup */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Backup    "] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (backupClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"upload_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Restore */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Restore    "] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (restoreClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"download_32.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		

	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier] autorelease];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];		
	
	/* Performance */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Performance"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (performanceClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"view_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem label]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	[toolbar setSelectedItemIdentifier:@"Status"];
	
	[window setToolbar:toolbar];
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

- (void) statusClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"status"];
}

- (void) customersClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"customers"];
	[customerController refreshCustomerList];
}

- (void) licensesClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"licenses"];
	[customerController refreshCustomerList];
	[licenseController refreshLicenseKeyList];
}

- (void) modulesClicked:(id)sender
{
	[modulesController refreshModules:self];
	[tabView selectTabViewItemWithIdentifier:@"modules"];
}

- (void) configureClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"configure"];
}

- (void) updateClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"update"];
	if (!updateController.recentlyUpdated)
	{ [updateController checkForUpdateClicked:self]; }
}

- (void) backupClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"backup"];
}

- (void) restoreClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"restore"];
}

- (void) diagnosticClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"diagnostic"];
}

- (void) performanceClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"performance"];
}


@end


