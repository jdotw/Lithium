//
//  LTIncidentList.h
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTIncident.h"
#import "LTAction.h"
#import "LTAPIRequest.h"

@interface LTIncidentList : LTAPIRequest 
{
	NSMutableArray *incidents;
	NSMutableDictionary *incidentDict;
	
	BOOL historicList;
	int maxResultsCount;
	LTEntity *entity;
	
	BOOL refreshCountOnly;
	int currentRequest;			// 1 = CountAndVersion, 2 = Full List
	
	unsigned long listVersion;
	unsigned long incidentCount;
}

- (void) refreshCountOnly;
- (void) refresh;
- (void) forceRefresh;

@property (nonatomic,readonly) unsigned long incidentCount;
@property (nonatomic,readonly) NSMutableArray *incidents;
@property (nonatomic,readonly) NSMutableDictionary *incidentDict;
@property (nonatomic,assign) BOOL historicList;
@property (nonatomic,assign) int maxResultsCount;
@property (nonatomic,retain) LTEntity *entity;
@property (nonatomic,retain) NSDate *lastRefresh;

@end
