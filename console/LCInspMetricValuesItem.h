//
//  LCInspMetricValuesItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"
#import "LCInspMetricValuesViewController.h"

@interface LCInspMetricValuesItem : LCInspectorItem 
{
}

+ (LCInspMetricValuesItem *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController;
- (LCInspMetricValuesItem *) initWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController;

@end
