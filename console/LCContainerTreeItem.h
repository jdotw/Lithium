//
//  LCContainerTreeItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"

@interface LCContainerTreeItem : NSObject 
{
	LCDevice *device;
	
	float rowHeight;
	NSMutableArray *children;
	NSPredicate *childrenFilter;
	BOOL isContainerTreeLeaf;
}

#pragma mark Accessors
@property (retain) LCDevice *device;
@property (readonly) NSString *displayString;
@property (assign) float rowHeight;
@property (copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (assign) BOOL isContainerTreeLeaf;
@property (readonly) BOOL selectable;
@property (readonly) int opState;


@end
