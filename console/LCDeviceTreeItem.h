//
//  LCDeviceTreeItem.h
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCDeviceTreeItem : NSObject 
{
	float rowHeight;
	NSMutableArray *children;
	BOOL isDeviceTreeLeaf;
}

#pragma mark Properties
@property (readonly) NSString *displayString;
@property (assign) float rowHeight;
@property (copy) NSMutableArray *children;
- (void) insertObject:(id)obj inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (assign) BOOL isDeviceTreeLeaf;
@property (readonly) BOOL selectable;
@property (readonly) int opState;

@end
