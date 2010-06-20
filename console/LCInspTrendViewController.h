//
//  LCInspTrendViewController.h
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCTransparentOutlineView.h"
#import "LCAnalysisMetric.h"

@interface LCInspTrendViewController : LCInspectorViewController
{
	NSMutableArray *analysisMetrics;
	IBOutlet LCTransparentOutlineView *outlineView;
}

@property (readonly) NSMutableArray *analysisMetrics;
- (void) insertObject:(LCAnalysisMetric *)metric inAnalysisMetricsAtIndex:(unsigned int)index;
- (void) removeObjectFromAnalysisMetricsAtIndex:(unsigned int)index;

@end
