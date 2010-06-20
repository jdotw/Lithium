//
//  LCBrowserApplicationContentController.h
//  Lithium Console
//
//  Created by James Wilson on 16/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"
#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCObjectTree.h"
#import "LCInspectorController.h"
#import "LCObjectTreeOutlineView.h"
#import "LCGraphStadiumController.h"
#import "LCBrowserHorizontalScroller.h"

@interface LCBrowserApplicationContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCDevice *device;
	NSMutableArray *objects;
	LCObjectTree *objectTree;
	LCBrowser2Controller *browser;
	
	/* UI Elements */
	IBOutlet NSTreeController *objectTreeController;
	IBOutlet LCObjectTreeOutlineView *objectOutlineView;
	IBOutlet LCInspectorController *inspectorController;
	IBOutlet LCGraphStadiumController *stadiumController;
	
	/* Selection */
	LCEntity *selectedEntity;
	NSArray *selectedEntities;
	BOOL doNotScrollFlow;
	
	/* Misc */
	NSString *stadiumTitle;
}

#pragma mark "Constructors"
- (id) initWithBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (assign) LCEntity *selectedEntity;
@property (copy) NSArray *selectedEntities;
- (void) selectEntity:(LCEntity *)entity;

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

#pragma mark "Properties"
@property (readonly) NSMutableArray *objects;
- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index;
- (void) removeObjectFromObjectsAtIndex:(unsigned int)index;
@property (retain) LCObjectTree *objectTree;
@property (readonly) NSWindow *window;
@property (assign) LCBrowser2Controller *browser;
@property (assign) BOOL doNotScrollFlow;
@property (copy) NSString *stadiumTitle;

@end
