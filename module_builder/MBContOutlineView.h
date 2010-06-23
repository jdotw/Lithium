//
//  MBContOutlineView.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"
#import "MBTrigger.h"
#import "MBMetric.h"
#import "MBContainer.h"

@interface MBContOutlineView : LCTransparentOutlineView 
{
	NSTableColumn *viewColumn;
	NSMutableDictionary *viewControllers;
	IBOutlet NSTreeController *treeController;
}

#pragma mark "Selection"
- (void) selectTrigger:(MBTrigger *)trigger;
- (void) selectMetric:(MBMetric *)metric;
- (void) selectContainer:(MBContainer *)container;


@end
