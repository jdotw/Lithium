//
//  LCInspMetricValuesViewController.h
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCBrowserTableView.h"

@interface LCInspMetricValuesViewController : LCInspectorViewController 
{
	IBOutlet LCBrowserTableView *tableView;
	NSArray *metrics;
}

+ (LCInspMetricValuesViewController *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics;
- (LCInspMetricValuesViewController *) initWithTarget:(id)initTarget metrics:(NSArray *)initMetrics;

@property (nonatomic,copy) NSArray *metrics;

@end
