//
//  LCVRackWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 11/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackController.h"
#import "LCBackgroundView.h"
#import "LCContentView.h"

@interface LCVRackWindowController : NSWindowController 
{
	id rackDocument;
	
	IBOutlet LCBackgroundView *backView;
	IBOutlet LCVRackController *rackController;
	IBOutlet NSObjectController *windowControllerAlias;
	IBOutlet NSDrawer *treeDrawer;
	IBOutlet NSDrawer *infoDrawer;
	BOOL infoShown;
	BOOL treeDrawerVisible;
	BOOL infoDrawerVisible;
	BOOL cableListPanelVisible;
	BOOL cableGroupPanelVisible;
	IBOutlet NSPanel *cableListPanel;
	IBOutlet NSTableColumn *cableColorColumn;
	IBOutlet NSScrollView *scrollView;
	IBOutlet LCContentView *contentView;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;		
	
	LCVRackDocument *rack;
}

#pragma mark "Constructors"
- (LCVRackWindowController *) initWithRackDocument:(id)initDocument;
- (void) dealloc;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "Window Delegates"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "UI Actions"
- (IBAction) toggleShowCableStatusClicked:(id)sender;
- (IBAction) toggleDrawerClicked:(id)sender;
- (IBAction) toggleTreeDrawerClicked:(id)sender;
- (IBAction) toggleCableListPanelClicked:(id)sender;
- (IBAction) toggleCableGroupPanelClicked:(id)sender;

#pragma mark "Accessor Methods"
- (LCVRackController *) rackController;
- (BOOL) treeDrawerVisible;
- (void) setTreeDrawerVisible:(BOOL)flag;
- (BOOL) infoDrawerVisible;
- (void) setInfoDrawerVisible:(BOOL)flag;
- (BOOL) cableListPanelVisible;
- (void) setCableListPanelVisible:(BOOL)flag;
- (BOOL) cableGroupPanelVisible;
- (void) setCableGroupPanelVisible:(BOOL)flag;

@property (nonatomic,retain) id rackDocument;
@property (nonatomic,retain) LCBackgroundView *backView;
@property (retain,getter=rackController) LCVRackController *rackController;
@property (nonatomic,retain) NSObjectController *windowControllerAlias;
@property (nonatomic,retain) NSDrawer *treeDrawer;
@property (nonatomic,retain) NSDrawer *infoDrawer;
@property BOOL infoShown;
@property (getter=treeDrawerVisible,setter=setTreeDrawerVisible:) BOOL treeDrawerVisible;
@property (getter=infoDrawerVisible,setter=setInfoDrawerVisible:) BOOL infoDrawerVisible;
@property (getter=cableListPanelVisible,setter=setCableListPanelVisible:) BOOL cableListPanelVisible;
@property (getter=cableGroupPanelVisible,setter=setCableGroupPanelVisible:) BOOL cableGroupPanelVisible;
@property (nonatomic,retain) NSPanel *cableListPanel;
@property (nonatomic,retain) NSTableColumn *cableColorColumn;
@property (nonatomic,retain) NSScrollView *scrollView;
@property (nonatomic,retain) LCContentView *contentView;
@property (nonatomic,retain) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *toolbarItems;
@property (nonatomic,retain) NSMutableArray *toolbarSelectableItems;
@property (nonatomic,retain) NSMutableArray *toolbarDefaultItems;
@property (nonatomic,retain) LCVRackDocument *rack;
@end
