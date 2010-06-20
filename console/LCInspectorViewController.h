//
//  LCInspectorViewController.h
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorItem.h"

@interface LCInspectorViewController : LCInspectorItem 
{
	float rowHeight;
}

#pragma mark "Constructor"
- (LCInspectorViewController *) initWithTarget:(id)initTarget;

#pragma mark "Tree Methods"
- (BOOL) leafNode;

#pragma mark "Accessors"
- (float) defaultHeight;
- (float) rowHeight;
- (void) setRowHeight:(float)height;

@end
