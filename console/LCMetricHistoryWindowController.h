//
//  LCMetricHistoryWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 19/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCMetric.h"
#import "LCMetricHistory.h"
#import "LCBrowserTableView.h"
#import "LCBackgroundView.h"

@interface LCMetricHistoryWindowController : NSWindowController 
{
	LCMetric *metric;
	LCMetricHistory *history;
	
	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;

	/* Toolbar views */
	IBOutlet NSView *dateSelectorView;
	IBOutlet NSView *periodSelectorView;
	IBOutlet NSView *refreshIndicatorView;	
	
	/* UI Elements */
	IBOutlet LCBrowserTableView *tableView;
	IBOutlet LCBackgroundView *backView;
}

#pragma mark "Initialisation"
- (id) initWithMetric:(LCMetric *)initMetric;
- (void) dealloc;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "Toolbar Actions"
- (void) refreshClicked:(id)sender;
- (void) graphMetricClicked:(id)sender;
- (void) trendAnalysisClicked:(id)sender;
- (IBAction) exportToCSVClicked:(id)sender;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Accessors"
- (LCMetric *) metric;
- (void) setMetric:(LCMetric *)newEntity;

@property (nonatomic,retain,getter=history) LCMetricHistory *history;
@property (nonatomic,retain) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *toolbarItems;
@property (nonatomic,retain) NSMutableArray *toolbarSelectableItems;
@property (nonatomic,retain) NSMutableArray *toolbarDefaultItems;
@property (nonatomic,retain) NSView *dateSelectorView;
@property (nonatomic,retain) NSView *periodSelectorView;
@property (nonatomic,retain) NSView *refreshIndicatorView;
@property (nonatomic,retain) LCBrowserTableView *tableView;
@property (nonatomic,retain) LCBackgroundView *backView;
@end
