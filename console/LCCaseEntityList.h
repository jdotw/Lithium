//
//  LCCaseEntityList.h
//  Lithium Console
//
//  Created by James Wilson on 25/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntityDescriptor.h"
#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCCaseEntityList : NSObject 
{
	/* Objects */
	id cas;
	id delegate;
	
	/* Entity Descriptors */
	NSMutableArray *entities;
	NSMutableDictionary *entityDictionary;
	int highestEntityOpStateInteger;
	
	/* Refresh variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL xmlParseInProgress;
	NSMutableDictionary *curXMLDictionary;
	NSMutableString *curXMLString;	
	NSMutableDictionary *xmlEntityDictionary;
	BOOL refreshInProgress;
}

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;
- (void) updateHighestEntityOpState;

#pragma mark "Accessor Methods"
@property (assign) id cas;
@property (assign) id delegate;
@property (readonly) NSMutableArray *entities;
- (void) insertObject:(LCEntity *)entity inEntitiesAtIndex:(unsigned int)index;
- (void) removeObjectFromEntitiesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *entityDictionary;
@property (assign) BOOL refreshInProgress;
@property (assign) int highestEntityOpStateInteger;

@end
