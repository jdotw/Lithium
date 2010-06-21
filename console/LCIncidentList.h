//
//  LCIncidentList.h
//  Lithium Console
//
//  Created by James Wilson on 29/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCEntity.h"
#import "LCIncident.h"


@interface LCIncidentList : NSObject 
{
	/* Related Objects */
	id customer;
	id delegate;
	
	/* List criteria */
	BOOL isLiveList;
	NSMutableDictionary *criteria;
	
	/* Incidents */
	NSMutableArray *incidents;
	NSMutableDictionary *incidentDictionary;
	NSMutableArray *newIncidents;
	NSMutableArray *clearedIncidents;
	
	/* Refresh variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableString *curXMLString;
	NSMutableDictionary *curXMLDictionary;
	NSMutableDictionary *xmlIncidentDictionary;
	LCIncident *curIncident;
	BOOL curIncidentIsNew;
	id curAction;
	NSMutableDictionary *curEntityDescDict;
	BOOL initialRefresh;
	BOOL refreshInProgress;
	BOOL processingAction;
	BOOL processingEntityDescriptor;
}

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Entity Searching"
- (NSMutableArray *) incidentsForEntity:(LCEntity *)entity;

#pragma mark "Criteria Accessor Methods"
@property (nonatomic,copy) NSNumber *incidentID;
@property (nonatomic,copy) NSNumber *stateInteger;
@property (nonatomic,copy) NSNumber *startUpperSeconds;
@property (nonatomic,copy) NSNumber *startLowerSeconds;
@property (nonatomic,copy) NSNumber *endUpperSeconds;
@property (nonatomic,copy) NSNumber *endLowerSeconds;
@property (nonatomic,retain) LCEntity *entity;
@property (nonatomic,copy) NSNumber *adminStateInteger;
@property (nonatomic,copy) NSNumber *opStateInteger;
@property (nonatomic,copy) NSNumber *caseID;
@property (nonatomic,copy) NSNumber *maxCount;

#pragma mark "Relevance Scoring"
- (void) scoreRelevanceToEntity:(LCEntity *)localEntity;

#pragma mark "Property Methods"
@property (nonatomic,retain) id customer;
@property (nonatomic, assign) id delegate;
@property (nonatomic, assign) BOOL isLiveList;
@property (readonly) NSMutableDictionary *criteria;
@property (readonly) NSMutableArray *incidents;
- (void) insertObject:(LCIncident *)inc inIncidentsAtIndex:(unsigned int)index;
- (void) removeObjectFromIncidentsAtIndex:(unsigned int) index;
@property (readonly) NSMutableDictionary *incidentDictionary;
@property BOOL initialRefresh;
@property BOOL refreshInProgress;


@end
