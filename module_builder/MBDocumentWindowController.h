//
//  MBDocumentWindowController.h
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBSnmpWalk.h"
#import "MBContainerTreeController.h"
#import "MBContOutlineView.h"
#import "MBMibArrayController.h"
#import "MBOidArrayController.h"
#import "MBContainer.h"
#import "MBModuleUploadRequest.h"
#import "MBTrigger.h"

@interface MBDocumentWindowController : NSWindowController <MBSnmpWalkDelegate>
{
	/* Scan */
	MBSnmpWalk *walk;
	BOOL scanInProgress;
	NSDate *scanDate;
	NSString *scanAddress;
	NSString *scanCommunity;
	NSString *scanResult;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;		
	
	/* UI Elements */
	IBOutlet NSTextField *scanDeviceAddressField;
	IBOutlet NSWindow *scanDeviceSheet;	
	IBOutlet MBContainerTreeController *containerTreeController;
	IBOutlet MBContOutlineView *containerOutlineView;
	IBOutlet NSView *searchView;
	IBOutlet MBMibArrayController *mibArrayController;
	IBOutlet MBOidArrayController *oidArrayController;
	IBOutlet NSOutlineView *oidOutlineView;
	IBOutlet NSObjectController *objectController;
	IBOutlet NSView *metricPropertiesView;
	IBOutlet NSView *leftContentView;
	IBOutlet NSTabView *leftTabView;
	IBOutlet NSSplitView *mainSplitView;
	IBOutlet NSMenu *newDynamicMetricMenu;
	
	/* Trigger */
	MBTrigger *newTrigger;
	MBMetric *newTriggerMetric;
	IBOutlet NSWindow *newTriggerSheet;
	IBOutlet NSTextField *newTriggerXValue;
	
	/* Module Export */
	IBOutlet NSWindow *exportSheet;
	IBOutlet NSWindow *exportProgressSheet;
	IBOutlet NSArrayController *coreDeploymentArrayController;
	NSMutableArray *exportUploadRequests;
	BOOL savePasswordToKeyChain;
	int completedRequests;
	int totalRequests;
	BOOL exportUploadInProgress;
	
	/* Container */
	MBContainer *activeContainer;
	
	/* Search */
	NSString *searchTabTag;
	NSString *searchString;
	NSArray *searchResults;
}

#pragma mark Init
- (MBDocumentWindowController *) init;

#pragma mark "Toolbar Methods"
- (IBAction) dummyClicked:(id)sender;
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag; 
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;
- (IBAction) toggleToolbarClicked:(id)sender;

#pragma mark UI Actions

#pragma mark Device Scanning Methods
@property (nonatomic, retain) MBSnmpWalk *walk;
- (IBAction) scanDeviceClicked:(id)sender;
- (IBAction) scanDeviceScanClicked:(id)sender;
- (IBAction) scanDeviceCancelClicked:(id)sender;
- (void) scanDidFinished:(BOOL)success error:(NSString *)error;
- (BOOL) scanInProgress;
- (void) setScanInProgress:(BOOL)flag;
- (NSDate *) scanDate;
- (void) setScanDate:(NSDate *)date;
- (NSString *) scanAddress;
- (void) setScanAddress:(NSString *)string;
- (NSString *) scanCommunity;
- (void) setScanCommunity:(NSString *)string;
- (NSString *) scanResult;
- (void) setScanResult:(NSString *)string;

#pragma mark MIB Loading Methods
- (IBAction) loadMIBClicked:(id)sender;
- (void)mibOpenSheetDidEnd:(NSOpenPanel *)panel returnCode:(int)returnCode  contextInfo:(void  *)contextInfo;

#pragma mark Container Methods
- (MBContainer *) selectedContainer;
- (NSArray *) selectedContainers;
- (IBAction) addNewContainerClicked:(id)sender;
- (IBAction) removeSelectedContainersClicked:(id)sender;
- (MBContainer *) activeContainer;
- (void) setActiveContainer:(MBContainer *)container;

#pragma mark "Metric Methods"
- (MBMetric *) selectedMetric;
- (NSArray *) selectedMetrics;
- (IBAction) newRateMetricClicked:(id)sender;
- (IBAction) newPercentMetricClicked:(id)sender;

#pragma mark "Trigger Methods"
- (MBTrigger *) selectedTrigger;
- (void) newTriggerClicked:(id)sender;
- (void) newTriggerForMetric:(MBMetric *)metric;
- (void) newTriggerAddClicked:(id)sender;
- (void) newTriggerCancelClicked:(id)sender;
- (void) showOverlapAlertSheetFor:(MBTrigger *)overlapTrigger;
@property (retain) MBTrigger *newTrigger;
@property (retain) MBMetric *newTriggerMetric;

#pragma mark "Module Export"
- (IBAction) exportToXMLClicked:(id)sender;
- (IBAction) exportSaveXMLToFileClicked:(id)sender;
- (void) exportSaveXMLToFileSheetEnded:(NSSavePanel *)sheet returnCode:(int)returnCode  contextInfo:(void  *)contextInfo;
- (IBAction) exportUploadClicked:(id)sender;
- (IBAction) exportCancelClicked:(id)sender;
- (IBAction) exportProgressCancelClicked:(id)sender;
- (IBAction) exportProgressCloseClicked:(id)sender;
@property (assign) BOOL savePasswordToKeyChain;
- (IBAction) exportAddDeploymentClicked:(id)sender;
- (IBAction) exportRemoveDeploymentClicked:(id)sender;
- (IBAction) exportRefreshDeploymentClicked:(id)sender;
- (void) saveCoreDeploymentList;
@property (copy) NSMutableArray *exportUploadRequests;
- (void) insertObject:(MBModuleUploadRequest *)obj inExportUploadRequestsAtIndex:(unsigned int)index;
- (void) removeObjectFromExportUploadRequestsAtIndex:(unsigned int)index;
@property (assign) int completedRequests;
@property (assign) int totalRequests;
@property (assign) BOOL exportUploadInProgress;

#pragma mark "Search"
@property (copy) NSString *searchTabTag;
@property (copy) NSString *searchString;
@property (copy) NSArray *searchResults;

#pragma mark "Feedback"
- (IBAction) sendFeedbackClicked:(id)sender;

#pragma mark Accessors
- (void)setDocument:(NSDocument *)document;

@end
