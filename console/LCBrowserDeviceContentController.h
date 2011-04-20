//
//  LCBrowserDeviceContentController.h
//  Lithium Console
//
//  Created by James Wilson on 23/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"
#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCContainerTree.h"
#import "LCObjectTree.h"
#import "LCInspectorController.h"
#import "LCContainerTreeOutlineView.h"
#import "LCObjectTreeOutlineView.h"
#import "LCGraphStadiumController.h"
#import "LCBrowserHorizontalScroller.h"
#import "LCAttachedWindowView.h"
#import "LCAttachedWindow.h"

@interface LCBrowserDeviceContentController : LCBrowser2ContentViewController
{
	/* Objects */
	LCDevice *device;
	LCContainerTree *containerTree;
	LCObjectTree *objectTree;
	NSTimer *refreshTimer;
	LCBrowser2Controller *browser;
	
	/* UI Elements */
	IBOutlet NSTreeController *containerTreeController;
	IBOutlet NSTreeController *objectTreeController;
	IBOutlet LCContainerTreeOutlineView *containerOutlineView;
	IBOutlet LCObjectTreeOutlineView *objectOutlineView;
	IBOutlet LCInspectorController *inspectorController;
	IBOutlet LCGraphStadiumController *stadiumController;
	IBOutlet NSButton *serviceMenuButton;
	IBOutlet NSButton *incidentPopUpButton;
	IBOutlet NSMenu *serviceMenu;
	IBOutlet NSMenuItem *serviceMenuEditItem;
	IBOutlet NSMenuItem *serviceMenuRemoveItem;
	IBOutlet NSButton *processMenuButton;
	IBOutlet NSMenu *processMenu;
	IBOutlet NSMenuItem *processMenuAddItem;
	IBOutlet NSMenuItem *processMenuEditItem;
	IBOutlet NSMenuItem *processMenuRemoveItem;
	IBOutlet NSMenu *incidentPopUpMenu;
	IBOutlet NSMenu *containerMenu;
	IBOutlet NSMenuItem *containerMenuTitleItem;
	IBOutlet NSMenu *objectMenu;
	IBOutlet NSMenuItem *objectMenuTitleItem;
	IBOutlet NSMenu *incidentPopUpAdjustMenu;
	IBOutlet NSButton *deviceSettingsButton;
	
	/* Availability Popup */
	IBOutlet LCAttachedWindowView *availAttachedWindowView;
	IBOutlet NSTextField *availAttachedWindowProtocolLabel;
	LCAttachedWindow *availAttachedWindow;
	BOOL availAttachedWindowShown;

	/* Selection */
	LCEntity *selectedEntity;
	NSArray *selectedEntities;
	BOOL doNotScrollFlow;
}

#pragma mark "Constructors"
- (id) initWithDevice:(LCDevice *)initDevice inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (nonatomic, assign) LCEntity *selectedEntity;
@property (nonatomic,copy) NSArray *selectedEntities;
- (void) selectEntity:(LCEntity *)entity;

#pragma mark "UI Actions"
- (IBAction) refreshDeviceClicked:(id)sender;
- (IBAction) graphSelectedClicked:(NSMenuItem *)sender;
- (IBAction) faultHistoryClicked:(NSMenuItem *)sender;
- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender;
- (IBAction) viewPropertiesClicked:(NSMenuItem *)sender;
- (IBAction) metricHistoryClicked:(id)sender;
- (IBAction) triggerTuningClicked:(id)sender;
- (IBAction) analyseSelectedClicked:(id)sender;
- (IBAction) copyWebUrlToClipBoardClicked:(id)sender;
- (IBAction) openWebUrlInBrowserClicked:(id)sender;
- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender;
- (IBAction) incidentPopUpClicked:(id)sender;
- (IBAction) servicePopUpClicked:(id)sender;
- (IBAction) servicePopUpAddClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpAddHTTPClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpAddDNSClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpAddSMTPClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpAddIMAPClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpAddPOPClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpEditClicked:(NSMenuItem *)sender;
- (IBAction) servicePopUpRemoveClicked:(NSMenuItem *)sender;
- (IBAction) processPopUpClicked:(id)sender;
- (IBAction) processPopUpAddClicked:(NSMenuItem *)sender;
- (IBAction) processPopUpEditClicked:(NSMenuItem *)sender;
- (IBAction) processPopUpRemoveClicked:(NSMenuItem *)sender;
- (IBAction) processPopUpEnableMonitoringClicked:(id)sender;
- (IBAction) removeServiceClicked:(id)sender;
- (IBAction) editServiceClicked:(id)sender;
- (IBAction) editDeviceClicked:(id)sender;
- (IBAction) editProcessProfileClicked:(id)sender;
- (IBAction) removeProcessProfileClicked:(id)sender;

#pragma mark "Properties"
@property (nonatomic,retain) LCDevice *device;
@property (nonatomic,retain) LCContainerTree *containerTree;
@property (nonatomic,retain) LCObjectTree *objectTree;
@property (readonly) NSWindow *window;
@property (nonatomic, assign) LCBrowser2Controller *browser;
@property (nonatomic, assign) BOOL doNotScrollFlow;

@end
