//
//  LCMetricAnalysisWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 24/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBackgroundView.h"
#import "LCEntity.h"
#import "LCContentView.h"
#import "LCOutlineView.h"
#import "LCAnalysisMetric.h"

@interface LCMetricAnalysisWindowController : NSWindowController 
{
	/* Objects */
	LCEntity *object;
	NSArray *objects;
	NSMutableArray *analysisMetrics;
	
	/* UI Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCBackgroundView *backView;
	IBOutlet NSView *objView;
	IBOutlet LCContentView *contentView;
	IBOutlet LCOutlineView *outlineView;
	IBOutlet NSTreeController *treeController;

	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
	NSToolbarItem *metricHistoryItem;
	
	/* Toolbar views */
	IBOutlet NSView *dateSelectorView;
	IBOutlet NSView *periodSelectorView;
	IBOutlet NSView *refreshIndicatorView;
		
	/* User variables */
	int datePeriod;
	NSDate *referenceDate;
	BOOL refreshInProgress;
}

#pragma mark "Constructors"
- (LCMetricAnalysisWindowController *) initWithObject:(LCEntity *)initObject;
- (void) dealloc;

#pragma mark "Window Delegate Methods"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;

#pragma mark "UI Actions"
- (IBAction) analyseClicked:(id)sender;
- (IBAction) metricHistoryClicked:(id)sender;
- (IBAction) graphSelectedClicked:(id)sender;
- (IBAction) graphObjectClicked:(id)sender;

#pragma mark "Accessors"
- (LCEntity *) object;
- (void) setObject:(LCEntity *)newObject;
- (NSMutableArray *) analysisMetrics;
- (void) insertObject:(LCAnalysisMetric *)metric inAnalysisMetricsAtIndex:(unsigned int)index;
- (void) removeObjectFromAnalysisMetricsAtIndex:(unsigned int)index;
- (int) datePeriod;
- (void) setDatePeriod:(int)period;
- (NSDate *) referenceDate;
- (void) setReferenceDate:(NSDate *)date;

- (BOOL) refreshInProgress;
-(void) setRefreshInProgress:(BOOL)flag;

@property (retain,getter=object,setter=setObject:) LCEntity *object;
@property (retain,getter=objects) NSArray *objects;
@property (retain,getter=analysisMetrics) NSMutableArray *analysisMetrics;
@property (retain) NSObjectController *controllerAlias;
@property (retain) LCBackgroundView *backView;
@property (retain) NSView *objView;
@property (retain) LCContentView *contentView;
@property (retain) LCOutlineView *outlineView;
@property (retain) NSTreeController *treeController;
@property (retain) NSToolbar *toolbar;
@property (retain) NSMutableDictionary *toolbarItems;
@property (retain) NSMutableArray *toolbarSelectableItems;
@property (retain) NSMutableArray *toolbarDefaultItems;
@property (retain) NSToolbarItem *metricHistoryItem;
@property (retain) NSView *dateSelectorView;
@property (retain) NSView *periodSelectorView;
@property (retain) NSView *refreshIndicatorView;
@property (getter=datePeriod,setter=setDatePeriod:) int datePeriod;
@property (retain,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@end
