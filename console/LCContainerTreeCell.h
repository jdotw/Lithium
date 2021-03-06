//
//  LCContainerTreeCell.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCShadowTextFieldCell.h"
#import "LCEntity.h"
#import "LCContainerTreeItem.h"

@interface LCContainerTreeCell : LCShadowTextFieldCell 
{
	id representedObject;
	BOOL lastRow;
	BOOL firstRow;
}

#pragma mark "Drawing Method";
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView;

#pragma mark "Root Item"
- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forRootItem:(LCContainerTreeItem *)item;

#pragma mark "Entity Drawing"
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity;

@property (nonatomic, assign) id representedObject;
@property (nonatomic, assign) BOOL lastRow;
@property (nonatomic, assign) BOOL firstRow;


@end
