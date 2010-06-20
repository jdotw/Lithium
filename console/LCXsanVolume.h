//
//  LCXsanVolume.h
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCXsanVolume : NSObject 
{
	/* Objects */
	NSMutableArray *objects;
	NSMutableDictionary *objectDictionary;
}

#pragma mark "Constructors"
+ (LCXsanVolume *) volumeWithObject:(LCEntity *)initEntity;
- (LCXsanVolume *) initWithObject:(LCEntity *)initEntity;
- (void) dealloc;

#pragma mark "Object Accessors"
- (NSMutableArray *) objects;
- (void) insertObject:(LCEntity *)object inObjectsAtIndex:(unsigned int)index;
- (void) removeObjectFromObjectsAtIndex:(unsigned int)index;
- (NSMutableDictionary *) objectDictionary;
- (LCEntity *) bestObject;

#pragma mark "Display Methods"
- (NSArray *) isBrowserTreeLeaf;
- (NSArray *) children;
- (NSString *) displayString;

@property (retain,getter=objects) NSMutableArray *objects;
@property (retain,getter=objectDictionary) NSMutableDictionary *objectDictionary;
@property (readonly) NSString *sortString;

@end
