//
//  LCActionListWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCActionList.h"

@interface LCActionListWindowController : NSWindowController 
{
	/* Customer */
	LCCustomer *customer; 
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
	NSToolbarItem *trendAnalysisItem;
	NSToolbarItem *metricHistoryItem;	
	
	/* Action List */
	LCActionList *actionList;
	
	/* UI Elements */
	IBOutlet NSArrayController *actionArrayController;
	IBOutlet NSWindow *deleteSheet;
	IBOutlet NSView *refreshView;
	IBOutlet LCBackgroundView *backgroundView;
}

#pragma mark "Constructors"
- (LCActionListWindowController *) initForCustomer:(LCCustomer *)initCustomer;
- (void) dealloc;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "UI Actions"
- (IBAction) addNewActionClicked:(id)sender;
- (IBAction) deleteActionClicked:(id)sender;
- (IBAction) editActionClicked:(id)sender;
- (IBAction) refreshListClicked:(id)sender;
- (IBAction) deleteSheetDeleteClicked:(id)sender;
- (IBAction) deleteSheetCancelClicked:(id)sender;
- (IBAction) manageScriptsClicked:(id)sender;

#pragma mark "Selection Management"
- (LCAction *) selectedAction;

#pragma mark "Accessors" 
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,retain) LCActionList *actionList;

@end
