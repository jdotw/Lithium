//
//  LCBWRepWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBWRepItemTreeController.h"

@interface LCBWRepWindowController : NSWindowController 
{
	id reportDocument;
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCBWRepItemTreeController *treeController;
	IBOutlet NSArrayController *arrayController;
	IBOutlet NSView *statsModeView;
	IBOutlet NSView *viewModeView;
	IBOutlet NSView *discardModeView;
	IBOutlet NSView *dateView;
	IBOutlet NSView *periodView;
	IBOutlet LCBackgroundView *backView;

	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;

	/* Refresh */
	IBOutlet NSWindow *refreshProgressSheet;
	BOOL refreshSheetShown;

}

#pragma mark "Constructors"
- (LCBWRepWindowController *) initWithReportDocument:(id)initDocument;
- (void) dealloc;

#pragma mark "Window Delegates"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "Toolbar Action Methods"
- (IBAction) newGroupClicked:(id)sender;
- (IBAction) refreshClicked:(id)sender;
- (IBAction) exportClicked:(id)sender;
#pragma mark "Document Accessors"
- (id) reportDocument;

#pragma mark "Selection Methods"
- (NSArray *) selectedObjects;
- (id) firstSelectedObject;

#pragma mark "Group Menu Methods"
- (IBAction) arrangeByDeviceClicked:(id)sender;
- (IBAction) removeGroupClicked:(id)sender;

#pragma mark "Common Menu Methods"
- (IBAction) browseToClicked:(id)sender;
- (IBAction) openCaseClicked:(id)sender;

#pragma mark "Interface Menu Methods"
- (IBAction) graphSelectedClicked:(id)sender;
- (IBAction) removeInterfaceClicked:(id)sender;

#pragma mark "Refresh Methods"
- (void) showRefreshSheet;
- (void) hideRefreshSheet;
- (BOOL) refreshSheetShown;
- (void) setRefreshSheetShown:(BOOL)value;
- (IBAction) cancelRefreshClicked:(id)sender;

@property (retain,getter=reportDocument) id reportDocument;
@property (retain) NSObjectController *controllerAlias;
@property (retain) LCBWRepItemTreeController *treeController;
@property (retain) NSArrayController *arrayController;
@property (retain) NSView *statsModeView;
@property (retain) NSView *viewModeView;
@property (retain) NSView *discardModeView;
@property (retain) NSView *dateView;
@property (retain) NSView *periodView;
@property (retain) LCBackgroundView *backView;
@property (retain) NSToolbar *toolbar;
@property (retain) NSMutableDictionary *toolbarItems;
@property (retain) NSMutableArray *toolbarSelectableItems;
@property (retain) NSMutableArray *toolbarDefaultItems;
@property (retain) NSWindow *refreshProgressSheet;
@property (getter=refreshSheetShown,setter=setRefreshSheetShown:) BOOL refreshSheetShown;
@end
