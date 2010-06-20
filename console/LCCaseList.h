//
//  LCCaseList.h
//  Lithium Console
//
//  Created by James Wilson on 7/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCEntity.h"

@interface LCCaseList : NSObject 
{
	/* Related objects */
	id customer;
	id delegate;
	
	/* List Criteria */
	NSMutableDictionary *criteria;
	
	/* Cases */
	NSMutableArray *cases;
	NSMutableDictionary *caseDictionary;
	
	/* Refresh Variables */
	LCXMLRequest *refreshXMLRequest;
	id curCase;
	NSMutableString *curXMLString;
	NSMutableDictionary *xmlCaseDictionary;
	BOOL refreshInProgress;
}

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Criteria Properites"
- (void) removeAllCriteria;
@property (retain) NSNumber *caseID;
@property (retain) NSNumber *stateInteger;
@property (retain) NSDate *startUpper;
@property (retain) NSDate *startLower;
@property (retain) NSDate *endUpper;
@property (retain) NSDate *endLower;
@property (retain) NSString *headline;
@property (retain) NSString *owner;
@property (retain) NSString *requester;
@property (retain) LCEntity *entity;

#pragma mark "Properties"
@property (retain) id customer;
@property (assign) id delegate;
@property (readonly) NSMutableArray *cases;
- (void) insertObject:(id)cas inCasesAtIndex:(unsigned int)index;
- (void) removeObjectFromCasesAtIndex:(unsigned int) index;
@property (readonly) NSMutableDictionary *caseDictionary;
@property (assign) BOOL refreshInProgress;

@end
