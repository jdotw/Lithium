//
//  LCIncmgrController.h
//  Lithium Console
//
//  Created by James Wilson on 27/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCCase.h"
#import "LCCaseLogEntry.h"
#import "LCSplitView.h"
#import "LCBrowserTableView.h"
#import "LCBackgroundView.h"
#import "LCBrowserFlatBack.h"

@interface LCIncmgrController : NSWindowController 
{
	/* Controllers */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSArrayController *customersArrayController;
	IBOutlet NSArrayController *incidentsArrayController;
	IBOutlet NSArrayController *casesArrayController;

	/* UI Elements */
	IBOutlet LCBrowserFlatBack *backgroundView;
    IBOutlet LCBrowserTableView *inc_tview;
	IBOutlet LCBrowserTableView *case_tview;
	IBOutlet NSWindow *p_window;
	IBOutlet id p_bar;
	IBOutlet id spin;
	BOOL p_window_active;
	IBOutlet NSView *filterIncidentsView;
	IBOutlet NSView *filterCasesView;
	IBOutlet NSView *searchView;
	IBOutlet LCSplitView *splitView;
	IBOutlet LCTableView *customerTableView;

	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *tb_items;
	NSMutableArray *tb_selitems;
	NSMutableArray *tb_defitems;
	NSToolbarItem *refinc_item;
	NSToolbarItem *refcase_item;
	NSToolbarItem *closecase_item;

	/* Log Sheet */
	IBOutlet NSWindow *l_window;
	IBOutlet NSTextView *l_entry;
	IBOutlet NSTextField *l_timefield;
	IBOutlet NSProgressIndicator *l_pbar;
	IBOutlet NSTextField *l_infofield;
	IBOutlet NSButton *l_recordbtn;
	IBOutlet NSButton *l_cancelbtn;
	
	/* Filter predicates */
	NSMutableArray *incidentFilterPredicates;
	NSMutableArray *caseFilterPredicates;
	IBOutlet NSArrayController *incidentFilterArrayControler;
	IBOutlet NSArrayController *caseFilterArrayController;
	NSMutableDictionary *incidentCustomSearchDictionary;
	NSMutableDictionary *caseCustomSearchDictionary;
	IBOutlet NSPopUpButton *incidentFilterPopUp;
	IBOutlet NSPopUpButton *caseFilterPopUp;
	unsigned int prevCaseFilterIndex;
	unsigned int prevIncidentFilterIndex;
	IBOutlet NSSearchField *searchField;
	NSString *searchString;
	
	/* Sort descriptor */
	NSArray *caseArraySortDescriptors;
	NSArray *incidentArraySortDescriptors;
	
	/* Table refresh timer */
	NSTimer *incidentRefreshTimer;
	
	/* Op variables */
	unsigned long logop_count;
	BOOL logop_closecase;
}

#pragma mark "Initialisation"
- (LCIncmgrController *) init;

#pragma mark "Case Action Methods"
- (IBAction) viewCaseClicked:(id)sender;
- (IBAction) newCaseClicked:(id)sender;
- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender;
- (IBAction) closeCaseClicked:(id)sender;
- (IBAction) updateCaseClicked:(id)sender;
- (void) updateSelectedCases:(BOOL)close_case;
- (IBAction) logRecordClicked:(id)sender;
- (void) caseClosedFinished:(LCCase *)sender;
- (void) logEntryRecordFinished:(LCCaseLogEntry *)sender;
- (IBAction) logCancelClicked:(id)sender;
-(void) endLogEntrySheet;

#pragma mark "Incident Action Methods"
- (IBAction) viewIncident:(id)sender;
- (IBAction) clearIncident:(id)sender;
- (IBAction) refreshIncidentsForSelectedIncident:(id)sender;
- (IBAction) browseToSelectedIncident:(id)sender;
- (IBAction) graphSelectedIncident:(id)sender;
- (IBAction) metricAnalyseSelectedIncident:(id)sender;
- (IBAction) faultHistoryForSelectedIncident:(id)sender;
- (IBAction) metricHistoryForSelectedIncident:(id)sender;
- (IBAction) triggerTuningForSelectedIncident:(id)sender;
- (IBAction) editActionsForSelectedIncident:(id)sender;
- (IBAction) cancelPendingActionsClicked:(id)sender;

#pragma mark "Customer Action Methods"
- (IBAction) refreshClicked:(id)sender;
- (IBAction) refreshIncidentsClicked:(id)sender;
- (IBAction) refreshCasesClicked:(id)sender;

#pragma mark "Shared UI Actions"
- (IBAction) lithiumSetupClicked:(id)sender;
- (IBAction) reviewActiveTriggersClicked:(id)sender;
- (IBAction) restartMonitoringProcessClicked:(id)sender;
- (IBAction) connectUsingSSH:(id)sender;
- (IBAction) connectUsingTelnet:(id)sender;
- (IBAction) connectUsingWeb:(id)sender;
- (IBAction) connectUsingARD:(id)sender;
- (IBAction) copyWebUrlToClipBoardClicked:(id)sender;
- (IBAction) openWebUrlInBrowserClicked:(id)sender;

#pragma mark "Table View Methods"
- (void) caseTableViewDoubleClick:(NSArray *)selectedCases;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "Accessor Methods"
- (NSArray *) customers;
- (NSArray *) incidentArraySortDescriptors;
- (void) setIncidentArraySortDescriptors:(NSArray *)array;
- (NSArray *) caseArraySortDescriptors;
- (void) setCaseArraySortDescriptors:(NSArray *)array;
- (NSString *) searchString;
- (void) setSearchString:(NSString *)flag;

@property (nonatomic,retain) NSObjectController *controllerAlias;
@property (nonatomic,retain) NSArrayController *customersArrayController;
@property (nonatomic,retain) NSArrayController *incidentsArrayController;
@property (nonatomic,retain) NSArrayController *casesArrayController;
@property (nonatomic,retain) LCBrowserTableView *inc_tview;
@property (nonatomic,retain) LCBrowserTableView *case_tview;
@property (nonatomic,retain) NSWindow *p_window;
@property (nonatomic,retain) id p_bar;
@property (nonatomic,retain) id spin;
@property (nonatomic,assign) BOOL p_window_active;
@property (nonatomic,retain) NSView *filterIncidentsView;
@property (nonatomic,retain) NSView *filterCasesView;
@property (nonatomic,retain) NSView *searchView;
@property (nonatomic,retain) LCSplitView *splitView;
@property (nonatomic,retain) LCTableView *customerTableView;
@property (nonatomic,retain) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *tb_items;
@property (nonatomic,retain) NSMutableArray *tb_selitems;
@property (nonatomic,retain) NSMutableArray *tb_defitems;
@property (nonatomic,retain) NSToolbarItem *refinc_item;
@property (nonatomic,retain) NSToolbarItem *refcase_item;
@property (nonatomic,retain) NSToolbarItem *closecase_item;
@property (nonatomic,retain) NSWindow *l_window;
@property (nonatomic,retain) NSTextView *l_entry;
@property (nonatomic,retain) NSTextField *l_timefield;
@property (nonatomic,retain) NSProgressIndicator *l_pbar;
@property (nonatomic,retain) NSTextField *l_infofield;
@property (nonatomic,retain) NSButton *l_recordbtn;
@property (nonatomic,retain) NSButton *l_cancelbtn;
@property (nonatomic,retain,getter=incidentFilterPredicates) NSMutableArray *incidentFilterPredicates;
@property (nonatomic,retain,getter=caseFilterPredicates) NSMutableArray *caseFilterPredicates;
@property (nonatomic,retain) NSArrayController *incidentFilterArrayControler;
@property (nonatomic,retain) NSArrayController *caseFilterArrayController;
@property (nonatomic,retain) NSMutableDictionary *incidentCustomSearchDictionary;
@property (nonatomic,retain) NSMutableDictionary *caseCustomSearchDictionary;
@property (nonatomic,retain) NSPopUpButton *incidentFilterPopUp;
@property (nonatomic,retain) NSPopUpButton *caseFilterPopUp;
@property (nonatomic,assign) unsigned int prevCaseFilterIndex;
@property (nonatomic,assign) unsigned int prevIncidentFilterIndex;
@property (nonatomic,retain) NSSearchField *searchField;
@property (nonatomic,retain) NSTimer *incidentRefreshTimer;
@property (nonatomic,assign) unsigned long logop_count;
@property (nonatomic,setter=updateSelectedCases:) BOOL logop_closecase;
@end
