//
//  LCInspMiniGraphItem.h
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorItem.h"
#import "LCInspMiniGraphViewController.h"

@interface LCInspMiniGraphItem : LCInspectorItem 
{
	
}

+ (LCInspMiniGraphItem *) itemForMinorMetricsWithTarget:(id)initTarget forController:(id)initController;
+ (LCInspMiniGraphItem *) itemWithTarget:(id)initTarget metrics:(NSArray *)initMetrics forController:(id)initController;
- (id) initForMinorMetricsWithTarget:(id)initTarget forController:(id)initController;
- (id) initForTarget:(id)target withMetrics:(NSArray *)initMetrics forController:(id)initController;

@end
