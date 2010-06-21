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

@property (nonatomic,retain,getter=object,setter=setObject:) LCEntity *object;
@property (nonatomic,retain,getter=objects) NSArray *objects;
@property (nonatomic,retain,getter=analysisMetrics) NSMutableArray *analysisMetrics;
@property (nonatomic,retain) NSObjectController *controllerAlias;
@property (nonatomic,retain) LCBackgroundView *backView;
@property (nonatomic,retain) NSView *objView;
@property (nonatomic,retain) LCContentView *contentView;
@property (nonatomic,retain) LCOutlineView *outlineView;
@property (nonatomic,retain) NSTreeController *treeController;
@property (nonatomic,retain) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *toolbarItems;
@property (nonatomic,retain) NSMutableArray *toolbarSelectableItems;
@property (nonatomic,retain) NSMutableArray *toolbarDefaultItems;
@property (nonatomic,retain) NSToolbarItem *metricHistoryItem;
@property (nonatomic,retain) NSView *dateSelectorView;
@property (nonatomic,retain) NSView *periodSelectorView;
@property (nonatomic,retain) NSView *refreshIndicatorView;
@property (nonatomic,getter=datePeriod,setter=setDatePeriod:) int datePeriod;
@property (nonatomic,retain,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (nonatomic,getter=refreshInProgress,setter=setRefreshInProgress:) BOOL refreshInProgress;
@end
