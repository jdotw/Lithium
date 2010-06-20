//
//  LCObjectTreeCell.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCObjectTreeItem.h"
#import "LCObjectTreeMetricItem.h"
#import "LCObjectTreeHeaderItem.h"
#import "LCShadowTextFieldCell.h"


@interface LCObjectTreeCell : LCShadowTextFieldCell 
{
	id representedObject;
	BOOL firstRow;
	BOOL expanded;
	BOOL lastRow;
}

#pragma mark "Drawing Method";
- (void) drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView;
- (void) drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity;
- (void) drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forMetrics:(LCObjectTreeMetricItem *)metricItem;

#pragma mark "Observed Object"
@property (assign) id representedObject;
@property (assign) BOOL firstRow;
@property (assign) BOOL lastRow;
@property (assign) BOOL expanded;

@end
