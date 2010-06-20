//
//  LCObjectTreeItem.h
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCObject.h"

@interface LCObjectTreeItem : NSObject 
{
	LCObject *object;

	float rowHeight;
	NSMutableArray *children;
	BOOL isObjectTreeLeaf;	
}

#pragma mark "Constructor"
- (id) initWithObject:(LCObject *)initObject;

#pragma mark Accessors
@property (retain) LCObject *object;
@property (readonly) LCEntity *entity;
@property (assign) float rowHeight;
@property (copy) NSMutableArray *children;
@property (assign) BOOL isObjectTreeLeaf;
- (NSString *) uniqueIdentifier;

@end
