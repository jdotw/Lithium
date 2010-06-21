//
//  LCInspectorController.h
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCInspectorOutlineView.h"
#import "LCInspectorPanel.h"
#import "LCInspectorItem.h"

@interface LCInspectorController : NSObject 
{
	id target;
	
	IBOutlet LCInspectorOutlineView *outlineView;
	
	NSMutableArray *items;
}

#pragma mark "Target Methods"
@property (nonatomic, retain) id target;

#pragma mark "Item Management"
- (void) resetItems;
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;

#pragma mark "Resize Arbitration"
- (void) adjustItemAbove:(LCInspectorItem *)item viewHeightsBy:(float)delta;
- (void) adjustItemAboveToDefaultHeight:(LCInspectorItem *)item;
- (void) adjustAllItemsToDefaultHeight;
- (BOOL) itemAboveAllowsResize:(LCInspectorItem *)item;
- (void) resizeDragFinished;

@end
