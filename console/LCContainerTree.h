//
//  LCContainerTree.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDevice.h"

@interface LCContainerTree : NSObject 
{
	/* Device */
	LCDevice *device;
	
	/* Items */
	NSMutableArray *items;
	
	/* Tree Controller */
	NSTreeController *treeController;
}

- (id) initWithDevice:(LCDevice *)initDevice;

@property (nonatomic,retain) LCDevice *device;
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)item inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;


@end
