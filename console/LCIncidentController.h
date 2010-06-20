//
//  IncidentViewerController.h
//  Lithium Console
//
//  Created by James Wilson on 18/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCIncident.h"
#import "LCIncidentList.h"
#import "LCCaseList.h"
#import "LCEntityTreeController.h"
#import "LCBrowserTableView.h"
#import "LCBackgroundView.h"
#import "LCContentView.h"
#import "LCAction.h"
#import "LCBrowser2ObjectTableView.h"
#import "LCObjectTree.h"
#import "LCObjectTreeOutlineView.h"

@interface LCIncidentController : NSWindowController 
{
	/* Incident */
	LCIncident *incident;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCEntityTreeController *containerTreeController;
	IBOutlet NSScrollView *entityScrollView;
	IBOutlet LCBrowserTableView *incidentTableView;
	IBOutlet LCBrowserTableView *caseTableView;
	IBOutlet LCBrowserTableView *similarTableView;	
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet LCContentView *topContentView;
	IBOutlet LCBrowserTableView *actionTableView;
	IBOutlet NSArrayController *actionArrayController;
	IBOutlet NSWindow *actionExecSheet;
	IBOutlet NSImageView *iconImageView;
	IBOutlet LCBrowser2ObjectTableView *objectTableView;
	IBOutlet LCObjectTreeOutlineView *objectOutlineView;

	/* Drawer */
	BOOL drawerVisible;

	/* Cases */
	LCCaseList *caseList;
	
	/* Incidents */
	LCIncidentList *incidentList;
	
	/* Object Tree */
	LCObjectTree *objectTree;
	
	/* Sort descriptor */
	NSArray *incidentArraySortDescriptors;
	NSArray *relatedIncidentArraySortDescriptors;
	NSArray *caseArraySortDescriptors;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;
	
	/* Related Incidents */
	LCIncidentList *relatedIncidentList;
	IBOutlet NSWindow *relatedSelectionWindow;
	IBOutlet NSButton *relatedSelectAllButton;
	BOOL selectAllUnSelects;
	
	/* Actions */
	NSTimer *actionRefreshTimer;
	LCAction *actionBeingExecuted;
}

#pragma mark "Initialisation"
- (LCIncidentController *) initForIncident:(LCIncident *)initIncident;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "Refresh Methods"
- (IBAction) refreshIncidentClicked:(id)sender;
- (IBAction) refreshEntityClicked:(id)sender;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Case Opening Methods"
- (IBAction) openCaseClicked:(id)sender;
- (IBAction) openCaseForMultipleClicked:(id)sender;
- (IBAction) relatedSelectionOpenClicked:(id)sender;
- (IBAction) relatedSelectionCancelClicked:(id)sender;
- (IBAction) relatedSelectionSelectDeviceClicked:(id)sender;
- (IBAction) relatedSelectionSelectSiteClicked:(id)sender;
- (IBAction) relatedSelectionSelectAllClicked:(id)sender;

#pragma mark "Action Methods"
- (void) refreshActionTableView;
- (IBAction) executeActionClicked:(id)sender;
- (IBAction) executeActionCloseClicked:(id)sender;
- (IBAction) editActionsClicked:(id)sender;

#pragma mark "Accessor Methods"
@property (retain) LCIncident *incident;
@property (readonly) LCCaseList *caseList;
@property (readonly) LCIncidentList *incidentList;
@property (readonly) LCObjectTree *objectTree;
@property (copy) NSArray *incidentArraySortDescriptors;
@property (copy) NSArray *relatedIncidentArraySortDescriptors;
@property (copy) NSArray *caseArraySortDescriptors;
@property (assign) BOOL drawerVisible;
@property (assign) LCAction *actionBeingExecuted;

@end
