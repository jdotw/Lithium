//
//  LCMetricGraphDocument.h
//  Lithium Console
//
//  Created by James Wilson on 7/10/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCMetricGraphView.h"
#import "LCMetricGraphController.h"
#import "LCMetricGraphEntityArrayController.h"
#import "LCBrowserTableView.h"
#import "LCBackgroundView.h"

@interface LCMetricGraphDocument : NSDocument 
{
	/* Graph controller */
	IBOutlet LCMetricGraphController *graphController;
	IBOutlet LCMetricGraphEntityArrayController *metricArrayController;
	IBOutlet LCMetricGraphView *graphView;
	IBOutlet LCBrowserTableView *tableView;
	IBOutlet LCBackgroundView *backgroundView;
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet NSObjectController *graphControllerAlias;
	IBOutlet NSScrollView *scrollView;
	IBOutlet NSWindow *datePickerSheet;
	
	/* Auto refresh timer */
	NSTimer *refreshTimer;
	
	/* Window Properties */
	NSRect windowFrame;
	
	/* Misc */
	int dateDropDownTag;
	NSDate *originalDate;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
	NSToolbarItem *trendAnalysisItem;
	NSToolbarItem *metricHistoryItem;
	
	/* Toolbar views */
	IBOutlet NSView *dateSelectorView;
	IBOutlet NSView *periodSelectorView;
	IBOutlet NSView *refreshIndicatorView;	
	
	/* Initial Values */
	NSArray *initialEntities;
	int initialGraphPeriod;
	NSDate *initialReferenceDate;
	BOOL documentLoading;
}

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "Toolbar Action Methods"
- (IBAction) refreshGraphClicked:(id)sender;

#pragma mark "Entity Action Methods"
- (IBAction) removeSelectedEntityClicked:(id)sender;
- (IBAction) refreshSelectedEntityClicked:(id)sender;
- (IBAction) graphSelectedEntityClicked:(id)sender;
- (IBAction) browseToSelectedEntityClicked:(id)sender;
- (IBAction) openCaseForSelectedEntityClicked:(id)sender;
- (IBAction) faultHistoryForSelectedEntityClicked:(id)sender;
- (IBAction) metricHistoryForSelectedEntityClicked:(id)sender;
- (IBAction) analyseSelectedEntityClicked:(id)sender;
- (IBAction) triggerTuningForSelectedEntityClicked:(id)sender;

#pragma mark "Date Picker"
- (IBAction) datePickerSelectClicked:(id)sender;
- (IBAction) datePickerCancelClicked:(id)sender;

#pragma mark "Property Methods"
@property (readonly) LCMetricGraphController *graphController;
@property (nonatomic,copy) NSArray *initialEntities;
@property (nonatomic,retain) LCEntity *initialEntity;
@property (nonatomic, assign) int initialGraphPeriod;
@property (nonatomic,copy) NSDate *initialReferenceDate;
@property (nonatomic, assign) NSRect windowFrame;
@property (nonatomic, assign) int dateDropDownTag;

@end
