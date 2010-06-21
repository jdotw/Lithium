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
@property (nonatomic, assign) int groupID;
@property (nonatomic, assign) int parentID;
@property (nonatomic, assign) LCGroup *parent;
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic,copy) NSString *name;
@property (nonatomic,copy) NSString *desc;
@property (readonly) NSMutableArray *children;
- (void) insertObject:(id)child inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *childrenDictionary;
@property (readonly) NSMutableArray *recentChildren;
@property (nonatomic,copy) NSDate *refreshVersion;

#pragma mark "Tree Item Properties"
@property (nonatomic,copy) NSString *displayString;
@property (nonatomic, assign) float rowHeight;
@property (nonatomic, assign) BOOL isBrowserTreeLeaf;
@property (readonly) NSImage *treeIcon;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (readonly) BOOL selectable;
@property (nonatomic, assign) int opState;
@property (readonly) NSString *uniqueIdentifier;

@end
