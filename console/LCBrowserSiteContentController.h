//
//  LCBrowserSiteContentController.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCDeviceTree.h"
#import "LCInspectorController.h"
#import "LCDeviceTreeOutlineView.h"

@interface LCBrowserSiteContentController : LCBrowser2ContentViewController 
{
	LCSite *site;
	LCDeviceTree *deviceTree;
	NSTimer *refreshTimer;
	LCBrowser2Controller *browser;
	
	/* UI Elements */
	IBOutlet NSTreeController *deviceTreeController;
	IBOutlet LCDeviceTreeOutlineView *deviceOutlineView;
	IBOutlet LCInspectorController *inspectorController;

	/* Selection */
	LCEntity *selectedDevice;
	NSArray *selectedDevices;	
	LCIncident *selectedIncident;
	NSArray *selectedIncidents;
	id inspectorTarget;
}

#pragma mark "Constructors"
- (id) initWithSite:(LCSite *)initSite inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (assign) LCEntity *selectedDevice;
@property (copy) NSArray *selectedDevices;
@property (assign) LCIncident *selectedIncident;
@property (copy) NSArray *selectedIncidents;
@property (assign) id inspectorTarget;

#pragma mark "UI Actions"
- (IBAction) graphSelectedClicked:(NSMenuItem *)sender;
- (IBAction) faultHistoryClicked:(NSMenuItem *)sender;
- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender;
- (IBAction) metricHistoryClicked:(id)sender;
- (IBAction) triggerTuningClicked:(id)sender;
- (IBAction) analyseSelectedClicked:(id)sender;
- (IBAction) copyWebUrlToClipBoardClicked:(id)sender;
- (IBAction) openWebUrlInBrowserClicked:(id)sender;
- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender;
- (IBAction) deviceOutlineViewDoubleClicked:(NSArray *)selectedItems;

#pragma mark "Properties"
@property (retain) LCSite *site;
@property (retain) LCDeviceTree *deviceTree;
@property (readonly) NSWindow *window;
@property (assign) LCBrowser2Controller *browser;

@end
