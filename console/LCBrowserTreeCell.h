//
//  LCBrowserTreeCell.h
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTextFieldCell.h"
#import "LCEntity.h"
#import "LCBrowserTreeItem.h"

@interface LCBrowserTreeCell : LCTextFieldCell 
{
	id representedObject;
	id parentRepresentedObject;
	BOOL outlineViewEnabled;
	BOOL selected;
}

#pragma mark "Drawing Method";
- (void) drawEntityStyleInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView opState:(int)opState bold:(BOOL)bold;
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView;

#pragma mark "Root Item"
- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forRootItem:(LCBrowserTreeItem *)item;

#pragma mark "Entity Drawing"
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forEntity:(LCEntity *)entity bold:(BOOL)bold;

#pragma mark "Group Entity Drawing"
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forGroupEntity:(LCEntity *)entity;

#pragma mark "Application Entity Drawing"
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forApplicationEntity:(LCEntity *)entity;

#pragma mark "Non-Entity Item Drawing"
- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forItem:(LCBrowserTreeItem *)item bold:(BOOL)bold;

#pragma mark "Fault Item Drawing"
- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forIncidents:(LCBrowserTreeItem *)item;
- (void)drawItemInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forCases:(LCBrowserTreeItem *)item;

#pragma mark "Other Object Drawing"
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView forOtherObject:(id)object;

#pragma mark "Observed Object"
@property (nonatomic, assign) id representedObject;
@property (nonatomic, assign) id parentRepresentedObject;
@property (nonatomic, assign) BOOL outlineViewEnabled;
@property (nonatomic, assign) BOOL selected;

@end
