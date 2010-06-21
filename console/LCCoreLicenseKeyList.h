//
//  LCCoreLicenseKeyList.h
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCCoreLicenseKey.h"

@interface LCCoreLicenseKeyList : NSObject 
{
	/* Related objects */
	LCCustomer *customer;
	
	/* Key list */
	NSMutableArray *keys;
	NSMutableDictionary *keyDictionary;

	/* Refresh Variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;	
}

#pragma mark "Construct"
+ (LCCoreLicenseKeyList *) keyListForCustomer:(LCCustomer *)initCustomer;
- (id) initForCustomer:(LCCustomer *)initCustomer;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Accessors"
@property (nonatomic,retain) LCCustomer *customer;
@property (readonly) NSMutableArray *keys;
- (void) insertObject:(LCCoreLicenseKey *)key inKeysAtIndex:(unsigned int)index;
- (void) removeObjectFromKeysAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *keyDictionary;
@property (nonatomic, assign) BOOL refreshInProgress;

@end
