//
//  LCGroup.h
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"

@interface LCGroup : LCXMLObject 
{
	/* Group Properties */
	int groupID;
	int parentID;
	LCGroup *parent;
	LCCustomer *customer;
	NSString *name;
	NSString *desc;
	NSMutableArray *children;
	NSMutableDictionary *childrenDictionary;
	NSMutableArray *recentChildren;				// Child entities seen in last refresh
	NSDate *refreshVersion;
	
	/* Tree item properties */
	int opState;
	float rowHeight;
	BOOL refreshInProgress;
	BOOL isBrowserTreeLeaf;
	NSString *displayString;
}

#pragma mark "Properties"
@property (assign) int groupID;
@property (assign) int parentID;
@property (assign) LCGroup *parent;
@property (retain) LCCustomer *customer;
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (readonly) NSMutableArray *children;
- (void) insertObject:(id)child inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *childrenDictionary;
@property (readonly) NSMutableArray *recentChildren;
@property (copy) NSDate *refreshVersion;

#pragma mark "Tree Item Properties"
@property (copy) NSString *displayString;
@property (assign) float rowHeight;
@property (assign) BOOL isBrowserTreeLeaf;
@property (readonly) NSImage *treeIcon;
@property (assign) BOOL refreshInProgress;
@property (readonly) BOOL selectable;
@property (assign) int opState;
@property (readonly) NSString *uniqueIdentifier;

@end
