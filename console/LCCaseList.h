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
@property (nonatomic,retain) NSNumber *caseID;
@property (nonatomic,retain) NSNumber *stateInteger;
@property (nonatomic,retain) NSDate *startUpper;
@property (nonatomic,retain) NSDate *startLower;
@property (nonatomic,retain) NSDate *endUpper;
@property (nonatomic,retain) NSDate *endLower;
@property (nonatomic,retain) NSString *headline;
@property (nonatomic,retain) NSString *owner;
@property (nonatomic,retain) NSString *requester;
@property (nonatomic,retain) LCEntity *entity;

#pragma mark "Properties"
@property (nonatomic,retain) id customer;
@property (nonatomic, assign) id delegate;
@property (readonly) NSMutableArray *cases;
- (void) insertObject:(id)cas inCasesAtIndex:(unsigned int)index;
- (void) removeObjectFromCasesAtIndex:(unsigned int) index;
@property (readonly) NSMutableDictionary *caseDictionary;
@property (nonatomic, assign) BOOL refreshInProgress;

@end
