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
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic, assign) BOOL isLicensed;
@property (nonatomic, assign) int maxDevices;
@property (nonatomic, assign) int deviceCount;
@property (nonatomic, assign) int excessDevices;
@property (nonatomic, assign) int expiry;
@property (nonatomic,copy) NSString *expiryString;
@property (nonatomic,copy) NSString *type;
@property (nonatomic,copy) NSString *typeString;
@property (nonatomic, assign) BOOL isNFR;
@property (nonatomic, assign) int demoType;
@property (nonatomic, assign) BOOL isDemo;
@property (nonatomic, assign) BOOL isExpiredDemo;
@property (nonatomic, assign) BOOL isFree;
@property (nonatomic, assign) BOOL isLimited;

@end
