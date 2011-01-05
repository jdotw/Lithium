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
	
	unsigned long listVersion;
	unsigned long incidentCount;
}

- (void) refreshCount;
- (void) refreshList;

@property (readonly) unsigned long incidentCount;
@property (readonly) NSMutableArray *incidents;
@property (readonly) NSMutableDictionary *incidentDict;
@property (assign) BOOL historicList;
@property (assign) int maxResultsCount;
@property (retain) LTEntity *entity;

@end
