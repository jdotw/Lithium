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
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic, assign) float rowHeight;
@property (nonatomic, copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (nonatomic, assign) BOOL isBrowserTreeLeaf;
@property (nonatomic,copy) NSImage *treeIcon;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic, assign) BOOL selectable;
@property (nonatomic, assign) int opState;
@property (nonatomic,copy) NSString *uniqueIdentifier;
@property (nonatomic,copy) NSString *sortString;

@end
