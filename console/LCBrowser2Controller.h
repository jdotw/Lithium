//
//  LCBrowser2Controller.h
//  Lithium Console
//
//  Created by James Wilson on 10/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomerList.h"
#import "LCEntityArrayController.h"
#import "LCBrowserTableView.h"
#import "LCBrowser2OutlineView.h"
#import "LCBrowser2ObjectOutlineView.h"
#import "LCSite.h"
#import "LCDevice.h"
#import "LCVendor.h"
#import "LCContentView.h"
#import "LCTransparentOutlineView.h"
#import "LCBrowserBackView.h"
#import "LCBrowserTree.h"
#import "LCBrowserTreeOutlineView.h"
#import "LCMetricGraphController.h"
#import "LCReviewActiveTriggersWindowController.h"
#import "LCInspectorController.h"
#import "LCBrowser2ContentView.h"
#import "LCBrowser2InspectorView.h"
#import "LCBrowserMidToolbarView.h"
#import "LCBrowser2ContentViewController.h"
#import "LCBrowserFlatBack.h"
#import "LCBrowserSearchContentController.h"

@interface LCBrowser2Controller : NSWindowController
{
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
	NSToolbarItem *metricHistoryItem;
	NSToolbarItem *graphSelectedItem;
	NSToolbarItem *analyseItem;
	NSToolbarItem *toggleDeviceViewItem;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSWindow *progressSheet;
	IBOutlet NSTextField *progressLabel;
	IBOutlet NSMenu *browserTreeSelectMenu;
	IBOutlet NSButton *entityTreeAddButton;
	IBOutlet NSButton *entityTreeSelectButton;
	IBOutlet NSMenuItem *browserAddSiteMenuItem;
	IBOutlet NSMenuItem *browserAddDeviceMenuItem;
	IBOutlet NSMenuItem *browserAddServiceMenuItem;
	IBOutlet LCBrowserBackView *backView;
	IBOutlet NSMenu *toolbarAddMenu;
	IBOutlet NSButton *barGraphButton;
	IBOutlet NSButton *barAnalyseButton;
	IBOutlet NSButton *barRefreshButton;
	IBOutlet NSSplitView *treeSplitView;
	IBOutlet NSView *treeSplitViewTreeView;
	IBOutlet NSView *treeSplitVIewContentView;
	IBOutlet NSSearchField *searchField;
	
	/* Add Menu UI Elements */
	IBOutlet NSMenu *browserAddMenu;
	IBOutlet NSMenuItem *addDeviceMenuItem;
	IBOutlet NSMenuItem *addSiteMenuItem;
	IBOutlet NSMenuItem *addServiceMenuItem;
	IBOutlet NSMenuItem *addActionMenuItem;	
	IBOutlet NSMenuItem *addSceneMenuItem;
	IBOutlet NSMenuItem *addVRackMenuItem;
	IBOutlet NSMenuItem *addGroupMenuItem;
	IBOutlet NSMenuItem *addImportCSVMenuItem;
	IBOutlet NSMenuItem *addImportARDMenuItem;
	
	/* Tree Selection */
	id treeSelectedObject;
	LCEntity *treeSelectedEntity;

	/* Content Panes */
	IBOutlet NSTabView *contentTabView;
	IBOutlet NSView *contentEnclosingView;
	IBOutlet LCBrowser2ContentView *topContentView;				/* Container for top content view */
	IBOutlet LCBrowser2ContentView *bottomContentView;			/* Container for bottom content view */
	IBOutlet LCBrowser2InspectorView *inspectorContentView;		/* Container for inspector contentview */
	IBOutlet NSView *bottomContentEnclosingView;				/* Toolbar and lower content */
	IBOutlet LCBrowserMidToolbarView *midToolbarView;			/* Toolbar */
	IBOutlet NSView *statusBarContentView;						/* Container for bottom status bar */
	LCBrowser2ContentViewController *contentViewController;
	BOOL inspectorShown;
	BOOL bottomContentShown;
	
	/* Browser Tree */
	LCBrowserTree *browserTree;
	IBOutlet NSTreeController *browserTreeController;
	IBOutlet LCBrowserTreeOutlineView *browserTreeOutlineView;
	
	/* Device-specific view */
//	LCEntity *deviceVisualEntity;
//	IBOutlet NSView *deviceUpperPaneView;
//	IBOutlet NSView *deviceLowerPaneView;
//	IBOutlet NSView *deviceVisualView;
//	id deviceVisual;
//	id outgoingDeviceVisual;
//	BOOL deviceViewCollapsed;
//	float deviceViewHeight;
//	NSViewAnimation *deviceSpecificViewAnimation;
//	int deviceViewMode;

	/* Sort Descriptors */
	NSMutableArray *browserSortDescriptors;
	NSArray *custSortDescriptors;
	NSArray *siteSortDescriptors;
	NSArray *deviceSortDescriptors;
	NSArray *containerSortDescriptors;
	NSArray *objectSortDescriptors;
	
	/* Filters */
	NSPredicate *entityPresenceFilter;
	NSPredicate *customerEnabledFilter;

	/* Search */
	NSString *searchString;
	LCBrowserSearchContentController *searchContentController;		/* The active search controller */
}

#pragma mark "Class Methods"
+ (NSMutableArray *) activeControllers;

#pragma mark "Initialisation"
- (LCBrowser2Controller *) init;
- (LCBrowser2Controller *) initWithEntity:(LCEntity *)entity;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "KVO Methods"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context;

#pragma mark "Tree Selection (Single Selection Only)"
@property (nonatomic,assign) id treeSelectedObject;
@property (nonatomic,assign) LCEntity *treeSelectedEntity;

#pragma mark "Global Selection Methods (Not KVO Observable)"
@property (nonatomic,readonly) LCEntity *selectedEntity;
@property (nonatomic,readonly) NSArray *selectedEntities;

#pragma mark "Selection Management"
- (void) selectEntity:(LCEntity *)entity;
- (void) selectIncident:(LCIncident *)incident;

#pragma mark "Content Pane Methods"
@property (nonatomic,retain) LCBrowser2ContentViewController *contentViewController;

#pragma mark "Toolbar Methods"
- (IBAction) dummyClicked:(id)sender;
- (void) buildToolbar;

#pragma mark "UI Actions"
- (IBAction) refreshSelectedClicked:(id)sender;
- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item;
- (IBAction) faultHistoryClicked:(NSMenuItem *)sender;
- (IBAction) refreshEntityClicked:(NSMenuItem *)sender;
- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender;
- (IBAction) viewPropertiesClicked:(NSMenuItem *)sender;
- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender;
- (IBAction) addNewSiteClicked:(id)sender;
- (IBAction) editSiteClicked:(id)sender;
- (IBAction) removeSiteClicked:(id)sender;
- (IBAction) addNewDeviceClicked:(id)sender;
- (IBAction) duplicateDeviceClicked:(id)sender;
- (IBAction) editDeviceClicked:(id)sender;
- (IBAction) removeDeviceClicked:(id)sender;
- (void) entityDoubleClicked:(NSArray *)entities;
- (void) incidentDoubleClicked:(NSArray *)incidents;
- (IBAction) actionListClicked:(id)sender;
- (IBAction) addNewActionClicked:(id)sender;
- (IBAction) addNewEmailActionClicked:(id)sender;
- (IBAction) manageActionScriptsClicked:(id)sender;
- (IBAction) restartMonitoringProcessClicked:(id)sender;
- (IBAction) restartCustomerProcessClicked:(id)sender;
- (IBAction) entityTreeAddButtonClicked:(id)sender;
- (IBAction) entityTreeSelectButtonClicked:(id)sender;
- (IBAction) manageLicensesClicked:(id)sender;
- (IBAction) reviewActiveTriggersClicked:(id)sender;
- (IBAction) resetTriggerRulesClicked:(id)sender;
- (IBAction) resetDeviceTriggerRulesClicked:(id)sender;
- (IBAction) preferencesClicked:(id)sender;
- (IBAction) errorLogClicked:(id)sender;
- (IBAction) markDeviceOutOfServiceClicked:(id)sender;
- (IBAction) markDeviceInProductionClicked:(id)sender;
- (IBAction) markDeviceInTestingClicked:(id)sender;
- (IBAction) renameSelectedDocumentClicked:(id)sender;
- (IBAction) deleteSelectedDocumentClicked:(NSMenuItem *)menuItem;
- (IBAction) addNewGroupToSelectedClicked:(id)sender;
- (IBAction) renameSelectedGroupClicked:(id)sender;
- (IBAction) deleteSelectedGroupClicked:(id)sender;
- (IBAction) removeSelectedEntityFromGroupClicked:(NSMenuItem *)menuItem;
- (IBAction) refreshIncidentsClicked:(id)sender;
- (IBAction) refreshCasesClicked:(id)sender;
- (IBAction) refreshServicesClicked:(id)sender;
- (IBAction) refreshProcessesClicked:(id)sender;
- (IBAction) refreshDocumentListClicked:(id)sender;
- (IBAction) refreshGroupsClicked:(id)sender;
- (IBAction) refreshDeploymentClicked:(NSMenuItem *)menuItem;

#pragma mark "LITHIUM.Web URL Methods"
- (IBAction) copyWebUrlToClipBoardClicked:(id)sender;
- (IBAction) openWebUrlInBrowserClicked:(id)sender;

#pragma mark "Connect-using Methods"
- (IBAction) connectUsingSSH:(id)sender;
- (IBAction) connectUsingTelnet:(id)sender;
- (IBAction) connectUsingWeb:(id)sender;
- (IBAction) connectUsingARD:(id)sender;

#pragma mark "Service Methods"
- (IBAction) addNewServiceClicked:(id)sender;
- (IBAction) addNewHTTPServiceClicked:(id)sender;
- (IBAction) addNewPOPServiceClicked:(id)sender;
- (IBAction) addNewSMTPServiceClicked:(id)sender;
- (IBAction) addNewIMAPServiceClicked:(id)sender;
- (IBAction) addNewDNSServiceClicked:(id)sender;
- (IBAction) manageServiceScriptsClicked:(id)sender;
- (IBAction) addNewProcessProfileClicked:(id)sender;

#pragma mark "Device Refresh Methods"
- (void) refreshDeviceTimerFired;

#pragma mark "Browser Tree Methods"
@property (nonatomic, retain) LCBrowserTree *browserTree;
@property (nonatomic, readonly) LCBrowserTreeOutlineView *browserTreeOutlineView;

#pragma mark "Search Methods"
@property (nonatomic, retain) NSString *searchString;
@property (nonatomic, retain) LCBrowserSearchContentController *searchContentController;

#pragma mark "Feedback Methods"
- (IBAction) sendFeedbackClicked:(id)sender;

#pragma mark "Accessors"
- (LCCustomerList *) customerList;
- (NSArray *) custSortDescriptors;
- (void) setCustSortDescriptors:(NSArray *)array;
- (NSArray *) siteSortDescriptors;
- (void) setSiteSortDescriptors:(NSArray *)array;
- (NSArray *) deviceSortDescriptors;
- (void) setDeviceSortDescriptors:(NSArray *)array;
- (NSArray *) containerSortDescriptors;
- (void) setContainerSortDescriptors:(NSArray *)array;
- (NSArray *) objectSortDescriptors;
- (void) setObjectSortDescriptors:(NSArray *)array;

@end
