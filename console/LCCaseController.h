/* LCCaseController */

#import <Cocoa/Cocoa.h>
#import "LCCase.h"
#import "LCCaseLogEntry.h"
#import "LCCaseLogEntryList.h"
#import "LCCaseEntityArrayController.h"
#import "LCBrowserTableView.h"
#import "LCBackgroundView.h"
#import "LCContentView.h"

@interface LCCaseController : NSWindowController
{
	/* Case */
	LCCase *cas;
		
	/* Controllers */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSArrayController *logEntriesController;
	IBOutlet LCCaseEntityArrayController *entitiesController;
	IBOutlet LCCaseEntityArrayController *openEntitiesController;
	
	/* Main viewer */
    IBOutlet NSTextField *hline;
    IBOutlet NSTextField *customer;
    IBOutlet NSTextField *casenum;
    IBOutlet NSTextField *owner;
    IBOutlet NSTextField *state;
	IBOutlet NSTextField *opened;
    IBOutlet NSTextField *lastupdated;
    IBOutlet NSTextField *lastcontact;
    IBOutlet NSTextField *closed;	
    IBOutlet LCBrowserTableView *entityTableView;
    IBOutlet LCBrowserTableView *logTableView;
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet LCContentView *contentView;
	
	/* Progress Sheet */
    IBOutlet NSWindow *progressWindow;
    IBOutlet NSTextField *progressInfoField;
    IBOutlet NSProgressIndicator *progressProgressBar;

	/* Log Sheet */
	IBOutlet NSWindow *logWindow;
	IBOutlet NSTextView *logEntry;
	IBOutlet NSTextField *logTimeSpent;
	IBOutlet NSProgressIndicator *logProgressBar;
	IBOutlet NSTextField *logInfoField;
	IBOutlet NSButton *logRecordButton;
	IBOutlet NSButton *logCancelButton;
	
	/* Open case sheet */
	IBOutlet NSWindow *openWindow;
	IBOutlet NSComboBox *openCustomer;
	IBOutlet NSTextField *openRequester;
	IBOutlet NSTextField *openHeadline;
	IBOutlet NSTableView *openEntityTableView;
	IBOutlet NSTextView *openLogEntry;
	IBOutlet NSButton *openOKButton;
	IBOutlet NSButton *openCancelButton;
	IBOutlet NSProgressIndicator *openProgressBar;
	IBOutlet NSTextField *openInfoField;
	NSMutableArray *openEntityArray;
	NSString *openCustomerString;
	NSString *openRequesterString;
	NSString *openHeadlineString;

	/* Re-Assign Sheet */
	IBOutlet NSWindow *reassignSheet;
	IBOutlet NSTextField *reassignTextField;
	NSString *reassignNameString;
	
	/* Change headline sheet */
	IBOutlet NSWindow *changeHeadlineSheet;
	IBOutlet NSTextField *changeHeadlineTextField;
	NSString *changeHeadlineString;
	
	/* ReOpen Sheet */
	IBOutlet NSWindow *reOpenSheet;
	IBOutlet NSTextView *reOpenTextView;
	IBOutlet NSProgressIndicator *reOpenProgressBar;
	IBOutlet NSTextField *reOpenInfoField;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;
	
	/* Refresh */
	BOOL caseAutoRefresh;
	NSTimer *caseRefreshTimer;
	BOOL logEntryListAutoRefresh;
	NSTimer *logEntryListRefreshTimer;
	BOOL entityListAutoRefresh;
	NSTimer *entityListRefreshTimer;
	
	/* Misc variables */
	NSMutableArray *rowHeightArray;
	BOOL logOperationCloseCase;
	unsigned long refreshOperationCount;
}

#pragma mark "Initialisation"
- (LCCaseController *) initWithCase:(LCCase *)initcase;
- (LCCaseController *) initForNewCaseAtCustomer:(LCCustomer *)initCustomer;
- (LCCaseController *) initForNewCase;
- (LCCaseController *) initForNewCaseWithEntityList:(NSArray *)entarray;
- (LCCaseController *) init;

#pragma mark "Case opening"
- (IBAction) openOpenClicked:(id)sender;
- (IBAction) openCancelClicked:(id)sender;
- (void) caseOpenFinished:(LCCase *)sender;

#pragma mark "Case closing"
-(IBAction) closeCaseClicked:(id)sender;
- (void) caseClosedFinished:(LCCase *)sender;

#pragma mark "Entities"
- (IBAction) graphSelectedEntityClicked:(id)sender;
- (IBAction) browseToSelectedEntityClicked:(id)sender;
- (IBAction) faultHistoryForSelectedEntityClicked:(id)sender;
- (IBAction) metricHistoryForSelectedEntityClicked:(id)sender;
- (IBAction) triggerTuningForSelectedEntityClicked:(id)sender;

#pragma mark "Log Entries"
- (IBAction) updateLogClicked:(id)sender;
- (IBAction) logRecordClicked:(id)sender;
- (IBAction) logCancelClicked:(id)sender;
-(void) endLogEntrySheet;
- (NSString *) logEntryDisplayString:(LCCaseLogEntry *)log;

#pragma mark "TableView Methods"
- (float)tableView:(NSTableView *)tableView heightOfRow:(int)row;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "Refresh Routines"
@property (assign) BOOL logEntryListAutoRefresh;
@property (retain) NSTimer *logEntryListRefreshTimer;
@property (assign) BOOL entityListAutoRefresh;
@property (retain) NSTimer *entityListRefreshTimer;
- (IBAction) refreshEntitiesClicked:(id)sender;
- (IBAction) refreshLogEntriesClicked:(id)sender;

#pragma mark "Case Assignment Methods"
- (IBAction) reassignClicked:(id)sender;
- (IBAction) reassignSheetAssignClicked:(id)sender;
- (IBAction) reassignSheetCancelClicked:(id)sender;

#pragma mark "Headline Change Methods"
- (IBAction) changeHeadlineClicked:(id)sender;
- (IBAction) changeHeadlineSaveClicked:(id)sender;
- (IBAction) changeHeadlineCancelClicked:(id)sender;

#pragma mark "Case Re-opening Methods"
- (IBAction) reOpenCaseClicked:(id)sender;
- (IBAction) reOpenCaseOpenClicked:(id)sender;
- (IBAction) reOpenCaseFinished:(id)sender;
- (IBAction) reOpenCaseCancelClicked:(id)sender;

#pragma mark "Properties"
@property (retain) LCCase *cas;
@property (readonly) NSMutableArray *openEntityArray;
- (void) insertObject:(LCEntity *)entity inOpenEntityArrayAtIndex:(unsigned int)index;
- (void) removeObjectFromOpenEntityArrayAtIndex:(unsigned int)index;
@property (copy) NSString *openCustomerString;
@property (copy) NSString *openRequesterString;
@property (copy) NSString *openHeadlineString;


@end
