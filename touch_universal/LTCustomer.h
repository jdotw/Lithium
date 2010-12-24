//
//  LTCustomer.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"
#import "LTIncidentList.h"

@interface LTCustomer : LTEntity 
{
	NSString *url;
	NSString *cluster;
	NSString *node;
	LTIncidentList *incidentList;
	
	id groupTree;
}

@property (copy) NSString *url;
@property (copy) NSString *cluster;
@property (copy) NSString *node;
@property (retain) LTIncidentList *incidentList;
@property (retain) id groupTree;
@property (nonatomic, retain) NSMutableDictionary *customModules;

@property (nonatomic, readonly) int coreVersionMajor;
@property (nonatomic, readonly) int coreVersionMinor;
@property (nonatomic, readonly) int coreVersionPoint;

@end
