//
//  LCIPRegistryNetworkList.h
//  Lithium Console
//
//  Created by James Wilson on 2/10/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCXMLRequest.h"
#import "LCIPRegistryNetwork.h"

@interface LCIPRegistryNetworkList : NSObject 
{
	/* Customer */
	LCCustomer *customer;
	NSString *displayString;
	
	/* Delegate */
	id delegate;

	/* Networks */
	NSMutableArray *children;
	NSMutableDictionary *childDictionary;

	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	LCIPRegistryNetwork *curNetwork;
	LCIPRegistryEntry *curEntry;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;		
	BOOL scanInProgress;
}

+ (LCIPRegistryNetworkList *) networkListForCustomer:(id)initCustomer;;
- (LCIPRegistryNetworkList *) initForCustomer:(id)initCustomer;
- (void) dealloc;
	
#pragma mark "Refresh Methods";
- (void) scanNetwork:(NSString *)address mask:(NSString *)mask;
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Properties"
@property (retain) LCCustomer *customer;
@property (copy) NSString *displayString;
- (void) updateDisplayString;
@property (readonly) NSMutableArray *children;
- (void) insertObject:(LCIPRegistryNetwork *)network inChildrenAtIndex:(unsigned int)index;
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *childDictionary;
@property (assign) BOOL refreshInProgress;
@property (assign) BOOL scanInProgress;
@property (assign) id delegate;
- (NSString *) uniqueIdentifier;

@end
