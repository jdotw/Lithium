//
//  LCBrowserTreeItem.h
//  Lithium Console
//
//  Created by James Wilson on 25/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

@interface LCBrowserTreeItem : NSObject 
{
	NSString *displayString;
	NSImage *treeIcon;
	int opState;
	float rowHeight;
	BOOL refreshInProgress;
	NSMutableArray *children;
	BOOL isBrowserTreeLeaf;
	BOOL selectable;
	NSString *uniqueIdentifier;
	NSString *sortString;
}

#pragma mark Accessors
@property (copy) NSString *displayString;
@property (assign) float rowHeight;
@property (copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (assign) BOOL isBrowserTreeLeaf;
@property (copy) NSImage *treeIcon;
@property (assign) BOOL refreshInProgress;
@property (assign) BOOL selectable;
@property (assign) int opState;
@property (copy) NSString *uniqueIdentifier;
@property (copy) NSString *sortString;

@end
