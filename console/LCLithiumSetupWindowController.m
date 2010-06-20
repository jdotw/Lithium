//
//  LCLithiumSetupWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 11/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCLithiumSetupWindowController.h"
#import "LCDemoRegoWindowController.h"

@implementation LCLithiumSetupWindowController

#pragma mark "Constructors"

+ (LCLithiumSetupWindowController *) setupControllerForCustomer:(LCCustomer *)initCustomer
{
	if ([initCustomer setupController])
	{ 
		[[[initCustomer setupController] window] makeKeyAndOrderFront:nil]; 
		return [initCustomer setupController];
	}
	return [[LCLithiumSetupWindowController alloc] initForCustomer:initCustomer showingTab:nil];
}

+ (LCLithiumSetupWindowController *) actionScriptSetupControllerForCustomer:(LCCustomer *)initCustomer
{
	if ([initCustomer setupController])
	{ 
		[[[initCustomer setupController] window] makeKeyAndOrderFront:nil]; 
		[[[initCustomer setupController] tabView] selectTabViewItemWithIdentifier:@"actionscripts"]; 
		[[[initCustomer setupController] toolbar] setSelectedItemIdentifier:@"actionscripts"];
		return [initCustomer setupController]; 
	}
	return [[LCLithiumSetupWindowController alloc] initForCustomer:initCustomer showingTab:@"actionscripts"];
}

+ (LCLithiumSetupWindowController *) serviceScriptSetupControllerForCustomer:(LCCustomer *)initCustomer
{
	if ([initCustomer setupController])
	{ 
		[[[initCustomer setupController] window] makeKeyAndOrderFront:nil]; 
		[[[initCustomer setupController] tabView] selectTabViewItemWithIdentifier:@"servicescripts"]; 
		[[[initCustomer setupController] toolbar] setSelectedItemIdentifier:@"servicescripts"];
		return [initCustomer setupController]; 
	}
	return [[LCLithiumSetupWindowController alloc] initForCustomer:initCustomer  showingTab:@"servicescripts"];
}

+ (LCLithiumSetupWindowController *) licenseSetupControllerForCustomer:(LCCustomer *)initCustomer
{
	if ([initCustomer setupController])
	{ 
		[[[initCustomer setupController] window] makeKeyAndOrderFront:nil]; 
		[[[initCustomer setupController] tabView] selectTabViewItemWithIdentifier:@"licensekeys"]; 
		[[[initCustomer setupController] toolbar] setSelectedItemIdentifier:@"licensekeys"];
		return [initCustomer setupController];
	}
	return [[LCLithiumSetupWindowController alloc] initForCustomer:initCustomer showingTab:@"licensekeys"];
}

- (LCLithiumSetupWindowController *) initForCustomer:(LCCustomer *)initCustomer showingTab:(NSString *)tabIdentifier
{
	/* Set customer */
	[self setCustomer:initCustomer];
	
	/* Load the NIB */
	[super initWithWindowNibName:@"LithiumSetupWindow"];
	[self window];
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	[self buildToolbar];
	
	/* Select Tab */
	if (tabIdentifier)
	{ 
		[tabView selectTabViewItemWithIdentifier:tabIdentifier]; 
		[toolbar setSelectedItemIdentifier:tabIdentifier];
	}
	
	/* Show */
	[[self window] makeKeyAndOrderFront:self];
	
	/* Check for demo */
	if ([[[customer properties] objectForKey:@"license_free"] intValue] == 1)
	{ 
		LCDemoRegoWindowController *controller;
		controller = (LCDemoRegoWindowController *) [[LCDemoRegoWindowController alloc] initForCustomer:customer];
		[NSApp beginSheet:[controller window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
	
	/* Set Master */
	[initCustomer setSetupController:self];
	
	/* Add Refresh Timers */
	coreInfoRefreshTimer = [[NSTimer scheduledTimerWithTimeInterval:5.0
															 target:coreInfoController
														   selector:@selector(highPriorityRefresh)
														   userInfo:nil
															repeats:YES] retain];
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Window Delegate Methods"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove timers */
	[coreInfoRefreshTimer invalidate];
	[coreInfoRefreshTimer release];
	
	/* Remove bindings/observers */
	[controllerAlias setContent:nil];
	[coreLicenseControllerAlias setContent:nil];
	[userControlerAlias setContent:nil];
	[actionControllerAlias setContent:nil];
	[serviceControllerAlias setContent:nil];
	[coreInfoControlerAlias setContent:nil];
	
	/* Autorelease */
	[customer setSetupController:nil];
	[self autorelease];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"PreferencesToolbar"];
	[toolbar setAutosavesConfiguration:NO];
	[toolbar setAllowsUserCustomization:NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Core Config */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Lithium Core Info"] autorelease];
	[tbitem setLabel:[tbitem itemIdentifier]];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (coreInfoClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Core.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	[toolbar setSelectedItemIdentifier:[tbitem itemIdentifier]];
	
	/* Users */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"users"] autorelease];
	[tbitem setLabel:@"User Management"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (usersClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"user_group_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	
	
	/* License Keys */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"licensekeys"] autorelease];
	[tbitem setLabel:@"License Keys"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (licenseKeysClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"certificate_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Action Scripts */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"actionscripts"] autorelease];
	[tbitem setLabel:@"Action Scripts"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (actionScriptsClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"tools_script_48.tif"]];
	[toolbarItems setObject:tbitem forKey:[tbitem itemIdentifier]];
	[toolbarSelectableItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarSelectableItems count]];	
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];	

	/* Service Scripts */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:@"servicescripts"] autorelease];
	[tbitem setLabel:@"Service Scripts"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (serviceScriptsClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"go_script_48.tif"]];
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

#pragma mark "Toolbar Methods"

- (IBAction) coreInfoClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"info"];
}

- (IBAction) usersClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"users"];
}

- (IBAction) licenseKeysClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"licensekeys"];
}

- (IBAction) actionScriptsClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"actionscripts"];
}

- (IBAction) serviceScriptsClicked:(id)sender
{
	[tabView selectTabViewItemWithIdentifier:@"servicescripts"];
}

#pragma mark "Accessors"

- (LCCustomer *) customer
{ return customer; }

- (void) setCustomer:(LCCustomer *)newCustomer
{
	[customer release];
	customer = [newCustomer retain];
}

- (NSTabView *) tabView
{ return tabView; }

- (NSToolbar *) toolbar
{ return toolbar; }

@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@synthesize backView;
@synthesize controllerAlias;
@synthesize coreLicenseControllerAlias;
@synthesize actionControllerAlias;
@synthesize serviceControllerAlias;
@synthesize userControlerAlias;
@synthesize coreInfoControlerAlias;
@synthesize tabView;
@synthesize coreInfoController;
@synthesize coreInfoRefreshTimer;
@end
