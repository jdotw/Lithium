//
//  LCObjectTreeOutlineView.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCTransparentOutlineView.h"
#import "LCObjectTreeCell.h"

@interface LCObjectTreeOutlineView : LCTransparentOutlineView 
{
	NSMutableArray *displayedSubviews;
}

#pragma mark "Selection"
- (void) selectObject:(LCObject *)obj;

@end
