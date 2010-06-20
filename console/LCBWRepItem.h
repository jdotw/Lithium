//
//  LCBWRepItem.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCBWRepItem : NSObject 
{	
	id parent;
	id reportDocument;

	NSMutableDictionary *properties;
}

#pragma mark Item Types

#define BWREP_ITEM_GROUP 1
#define BWREP_ITEM_DEVICE 2
#define BWREP_ITEM_INTERFACE 3

#pragma mark Constructors
- (LCBWRepItem *) init;
- (LCBWRepItem *) initWithEntity:(LCEntity *)initEntity;
- (void) awakeFromArchive:(id)document parent:(LCBWRepItem *)initParent;
- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder;

#pragma mark Tree Node Methods
- (BOOL) isLeafNode;

#pragma mark Properties Methods
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)value;

#pragma mark Parent Methods
- (id) parent;
- (void) setParent:(id)value;
- (void) moveToParent:(LCBWRepItem *)newParent;
- (id) parentGroup;

#pragma mark Type Methods
- (int) type;
- (void) setType:(int)value;

#pragma mark Children Methods
- (NSMutableArray *) children;
- (void) setChildren:(NSMutableArray *)value;
- (unsigned int) countOfChildren;
- (LCBWRepItem *) objectInChildrenAtIndex:(unsigned int)index;
- (void) insertObject:(LCBWRepItem *)item inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;

#pragma mark Entity Methods
- (LCEntity *) entity;
- (void) setEntity:(LCEntity *)value;

#pragma mark Placeholder Methods
- (BOOL) canEditDescription;
- (NSString *) displayDescription;
- (NSNumber *) inMinimum;
- (NSNumber *) inAverage;
- (NSNumber *) inMaximum;
- (NSNumber *) outMinimum;
- (NSNumber *) outAverage;
- (NSNumber *) outMaximum;

#pragma mark Report Document 
- (id) reportDocument;
- (void) setReportDocument:(id)doc;

@property (assign,getter=parent,setter=setParent:) id parent;
@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@end
