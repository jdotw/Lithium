/* LCConsoleController */

#import <Cocoa/Cocoa.h>
#import <Sparkle/Sparkle.h>

#import "LCIncmgrController.h"
#import "LCCustomerList.h"
#import "LCBrowser2Controller.h"

@interface LCConsoleController : NSObject
{
	NSMutableArray *persistentWindows;
	NSTimer *startupTimer;
	BOOL browserOpened;
	BOOL showingAssistant;
	
	NSOperationQueue *operationQueue;
	
	NSMutableArray *browsers;
	
	IBOutlet SUUpdater *updater;
	IBOutlet NSMenu *executeActionMenu;
	IBOutlet NSMenu *processMonitorMenu;
	IBOutlet NSMenu *addNewSiteMenu;
	IBOutlet NSMenu *addNewDeviceMenu;
	IBOutlet NSMenu *addNewActionMenu;
	IBOutlet NSMenu *addNewServiceMenu;
	IBOutlet NSMenu *addNewProcessMenu;
}

#pragma mark "Class Methods"
+ (LCConsoleController *) masterController;
+ (void) setMasterController:(LCConsoleController *)controller;

#pragma mark "Initialisation"
- (void) dealloc;

#pragma mark "Awake From NIB"
- (void) awakeFromNib;
- (void) thawPersistentWindows;
- (BOOL) showingAssistant;
- (void) setShowingAssistant:(BOOL)flag;
- (BOOL) browserOpened;
- (void) setBrowserOpened:(BOOL)flag;
- (void) hideNotificationWindow;

#pragma mark "Persistent Windows"
- (NSMutableArray *) persistentWindows;
- (void) insertObject:(NSWindowController *)windowController inPersistentWindowsAtIndex:(unsigned int)index;
- (void) removeObjectFromPersistentWindowsAtIndex:(unsigned int)index;
- (void) removePersistentWindow:(NSWindowController *)windowController;

#pragma mark "Browser Windows"
@property (readonly) NSMutableArray *browsers;
- (void) newBrowserDidOpen:(LCBrowser2Controller *)browser;
- (void) browserDidClose:(LCBrowser2Controller *)browser;
- (void) browserDidBecomeMainWindow:(LCBrowser2Controller *)browser;
- (void) insertObject:(LCBrowser2Controller *)browser inBrowsersAtIndex:(unsigned int)index;
- (void) removeObjectFromBrowsersAtIndex:(unsigned int)index;
- (LCBrowser2Controller *) browserForSheet;
	 
#pragma mark "Operation Queue"
@property (nonatomic,retain) NSOperationQueue *operationQueue;

#pragma mark "User Interface Methods"
- (IBAction) newIncidentManager:(id)sender;
- (IBAction) openNewCase:(id)sender;
- (IBAction) activityViewer:(id)sender;
- (IBAction) newBrowser:(id)sender;
- (IBAction) preferencesClicked:(id)sender;
- (IBAction) caseSearchClicked:(id)sender;
- (IBAction) newGraphWindow:(id)sender;
- (IBAction) newMultiGraphWindow:(id)sender;
- (IBAction) onlineDocsClicked:(id)sender;
- (IBAction) forumClicked:(id)sender;
- (IBAction) newVirtualRackClicked:(id)sender;
- (IBAction) newStateSceneClicked:(id)sender;
- (IBAction) showLicenseClicked:(id)sender;
- (IBAction) errorLogClicked:(id)sender;
- (IBAction) sendFeedbackClicked:(id)sender;
- (NSMenu *) executeActionMenu;



#pragma mark "Properties"
@property (readonly) NSMutableArray *persistentWindows;
@property (nonatomic,retain) NSTimer *startupTimer;
@property (nonatomic, assign) BOOL browserOpened;
@property (nonatomic, assign) BOOL showingAssistant;
@property (nonatomic,retain) SUUpdater *updater;
@property (nonatomic, assign) NSMenu *executeActionMenu;
@property (nonatomic, assign) NSMenu *processMonitorMenu;



@end
