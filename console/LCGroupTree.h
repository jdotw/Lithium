//
//  LCGroupTree.h
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"

@interface LCGroupTree : NSObject 
{
	/* Customer */
	id customer;
	
	/* Group */
	NSMutableArray *groups;
	NSMutableDictionary *groupDictionary;
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;	
}

#pragma mark "Constructors"
- (id) initWithCustomer:(id)initCustomer;

#pragma mark "Refresh"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Properties"
@property (retain) id customer;
@property (readonly) NSMutableArray *groups;
- (void) insertObject:(id)entity inGroupsAtIndex:(unsigned int)index;
- (void) removeObjectFromGroupsAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *groupDictionary;
@property (assign) BOOL refreshInProgress;

@end
