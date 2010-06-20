//
//  LCInspMiniGraphViewController.h
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorViewController.h"
#import "LCMetricGraphView.h"
#import "LCMetricGraphController.h"

@interface LCInspMiniGraphViewController : LCInspectorViewController
{
	IBOutlet LCMetricGraphView *graphView;
	IBOutlet LCMetricGraphController *graphController; 
	BOOL showDesc;
	NSArray *metrics;
}

+ (LCInspMiniGraphViewController *) itemWithTarget:(id)initTarget;
- (id) initWithTarget:(id)initTarget;

@property (assign) BOOL showDesc;
@property (readonly) NSArray *metrics;
@end
