//
//  LCCoreLicenseEntitlement.h
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"

@interface LCCoreLicenseEntitlement : LCXMLObject 
{
	/* Properties */
	BOOL isLicensed;
	int maxDevices;
	int deviceCount;
	int excessDevices;
	int expiry;
	NSString *expiryString;
	NSString *type;
	NSString *typeString;
	BOOL isNFR;
	int demoType;
	BOOL isDemo;
	BOOL isExpiredDemo;
	BOOL isFree;
	BOOL isLimited;
	
	/* Related objects */
	LCCustomer *customer;
	
	/* Refresh Variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;
}

#pragma mark "Constructors"
+ (LCCoreLicenseEntitlement *) entitlementForCustomer:(LCCustomer *)initCustomer;
- (id) initForCustomer:(LCCustomer *)initCustomer;

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Accessors"
@property (retain) LCCustomer *customer;
@property (assign) BOOL refreshInProgress;
@property (assign) BOOL isLicensed;
@property (assign) int maxDevices;
@property (assign) int deviceCount;
@property (assign) int excessDevices;
@property (assign) int expiry;
@property (copy) NSString *expiryString;
@property (copy) NSString *type;
@property (copy) NSString *typeString;
@property (assign) BOOL isNFR;
@property (assign) int demoType;
@property (assign) BOOL isDemo;
@property (assign) BOOL isExpiredDemo;
@property (assign) BOOL isFree;
@property (assign) BOOL isLimited;

@end
