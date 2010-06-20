//
//  LCObjectTree.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCContainer.h"
#import "LCObjectTreeOutlineView.h"

@interface LCObjectTree : NSObject 
{	
	/* Container */
	LCContainer *container;
	
	/* Items */
	NSMutableArray *items;
	
	/* OutlineView */
	LCObjectTreeOutlineView *outlineView;
}

- (id) initWithContainer:(LCContainer *)initContainer;
- (id) initWithObject:(LCObject *)initObject;

@property (retain) LCContainer *container;
@property (readonly) NSMutableArray *items;
- (void) insertObject:(id)item inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;


@end
