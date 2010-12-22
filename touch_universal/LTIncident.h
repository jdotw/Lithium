//
//  LTIncident.h
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntityDescriptor.h"

@interface LTIncident : LTAPIRequest 
{
	int identifier;
	NSDate *startDate;
	NSDate *endDate;
	int caseIdentifier;
	NSString *raisedValue;
	NSString *resourceAddress;
	LTEntityDescriptor *entityDescriptor;
	LTEntity *metric;
	NSMutableArray *actions;
	NSMutableDictionary *actionDict;
}

@property (assign) int identifier;
@property (copy) NSDate *startDate;
@property (copy) NSDate *endDate;
@property (assign) int caseIdentifier;
@property (copy) NSString *raisedValue;
@property (copy) NSString *resourceAddress;
@property (retain) LTEntityDescriptor *entityDescriptor;
@property (retain) LTEntity *metric;
@property (readonly) NSMutableArray *actions;
@property (readonly) NSMutableDictionary *actionDict;

@end
