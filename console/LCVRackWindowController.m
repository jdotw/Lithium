//
//  LCVRackWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackWindowController.h"
#import "LCColorCell.h"

@implementation LCVRackWindowController

#pragma mark "Constructors"

- (LCVRackWindowController *) initWithRackDocument:(id)initDocument
{
	/* Super-class init */
	[super initWithWindowNibName:@"VirtualRackWindow"];
	[self window];
	
	/* Set the rack */
	rack = initDocument;
	
	/* Set up rack controller */
	[rackController setRack:rack];
	
	/* Set back */
	[backView setImage:[NSImage imageNamed:@"slateback.png"]];
	[contentView setBackImage:nil];
	
	/* Show window */
	[[self window] setAcceptsMouseMovedEvents:YES];
	[[self window] setOpaque:NO];
	[[self window] setAspectRatio:[[self window] frame].size];
	[[self window] makeKeyAndOrderFront:self];
	
	/* Configure tableviews */
	[cableColorColumn setDataCell:[[[LCColorCell alloc] init] autorelease]];	
	
	/* Reset views */
	[rackController resetDeviceViews];
	[rackController resetCableViews];
	
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Toolbar Methods"

- (void) buildToolbar
{
	NSToolbarItem *tbitem;
	
	/* Create toolbar */
	toolbar = [[NSToolbar alloc] initWithIdentifier: @"BrowserToolbar"];
	[toolbar setAutosavesConfiguration:YES];
	[toolbar setAllowsUserCustomization: NO];
	[toolbar setDelegate: self];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeSmall];
	
	/* Create arrays/dict */
	toolbarItems = [[NSMutableDictionary dictionary] retain];	
	toolbarSelectableItems = [[NSMutableArray array] retain];
	toolbarDefaultItems = [[NSMutableArray array] retain];
	
	/* Drawer */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier: @"Toggle Drawer"] autorelease];
	[tbitem setLabel:@"Toggle Drawer"];
	[tbitem setTarget: self];
	[tbitem setAction: @selector (toggleDrawerClicked:)];
	[tbitem setImage:[NSImage imageNamed:@"Drawer.tiff"]];
	[toolbarItems setObject:tbitem forKey:@"Toggle Drawer"];
	[toolbarDefaultItems insertObject:[tbitem itemIdentifier] atIndex:[toolbarDefaultItems count]];
	
	/* Separator */
	tbitem = [[[NSToolbarItem alloc] initWithItemIdentifier:NSToolbarSeparatorItemIdentifier] autorelease];
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

#pragma mark "Window Delegates"

- (void) windowWillClose:(NSNotification *)notification
{
	/* Remove content */
	[windowControllerAlias setContent:nil];
	[rackController shutdown];
	
	/* Autorelease */
	[self autorelease];
}

#pragma mark "UI Actions"

- (IBAction) toggleShowCableStatusClicked:(id)sender
{
	if ([rackController showCableStatus])
	{ [rackController setShowCableStatus:NO]; }
	else
	{ [rackController setShowCableStatus:YES]; }
}

- (IBAction) toggleDrawerClicked:(id)sender
{
	if ([self infoDrawerVisible]) 
	{ [self setInfoDrawerVisible:NO]; }
	else
	{ [self setInfoDrawerVisible:YES]; 	}
}

- (IBAction) toggleTreeDrawerClicked:(id)sender
{
	if ([self treeDrawerVisible]) 
	{ [self setTreeDrawerVisible:NO]; }
	else
	{ [self setTreeDrawerVisible:YES]; 	}
}

- (IBAction) toggleCableListPanelClicked:(id)sender
{
	if ([self cableListPanelVisible])
	{ [self setCableListPanelVisible:NO]; }
	else
	{ [self setCableListPanelVisible:YES]; }
}

- (IBAction) toggleCableGroupPanelClicked:(id)sender
{
	if ([self cableGroupPanelVisible])
	{ [self setCableGroupPanelVisible:NO]; }
	else
	{ [self setCableGroupPanelVisible:YES]; }
}

#pragma mark "Accessor Methods"

- (LCVRackController *) rackController
{ return rackController; }

- (BOOL) treeDrawerVisible
{ return treeDrawerVisible; }
- (void) setTreeDrawerVisible:(BOOL)flag
{ 
	treeDrawerVisible = flag; 
	if (treeDrawerVisible)
	{ [treeDrawer openOnEdge:[treeDrawer preferredEdge]]; }
	else
	{ [treeDrawer close]; }
}

- (BOOL) infoDrawerVisible
{ return infoDrawerVisible; }
- (void) setInfoDrawerVisible:(BOOL)flag
{ 
	infoDrawerVisible = flag; 
	if (infoDrawerVisible)
	{ [infoDrawer openOnEdge:[infoDrawer preferredEdge]]; }
	else
	{ [infoDrawer close]; }	
}

- (BOOL) cableListPanelVisible;
{ return cableListPanelVisible; }
- (void) setCableListPanelVisible:(BOOL)flag
{ cableListPanelVisible = flag; }

- (BOOL) cableGroupPanelVisible
{ return cableGroupPanelVisible; }
- (void) setCableGroupPanelVisible:(BOOL)flag
{ cableGroupPanelVisible = flag; }


@synthesize rackDocument;
@synthesize backView;
@synthesize rackController;
@synthesize windowControllerAlias;
@synthesize treeDrawer;
@synthesize infoDrawer;
@synthesize infoShown;
@synthesize cableListPanel;
@synthesize cableColorColumn;
@synthesize scrollView;
@synthesize contentView;
@synthesize toolbar;
@synthesize toolbarItems;
@synthesize toolbarSelectableItems;
@synthesize toolbarDefaultItems;
@synthesize rack;
@end
