//
//  LCDevice.h
//  Lithium Console
//
//  Created by James Wilson on 24/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCProcessProfile.h"

@interface LCDevice : LCEntity 
{
	/* Properties */
	NSString *ipAddress;
	NSString *lomIpAddress;
	NSString *snmpCommunity;
	int snmpVersion;
	int snmpAuthMethod;
	int snmpPrivacyEncryption;
	NSString *snmpPrivPassword;
	NSString *snmpAuthPassword;
	NSString *username;
	NSString *password;
	NSString *lomUsername;
	NSString *lomPassword;
	NSString *profile;
	NSString *vendor;
	int refreshInterval;
	int osxProtocol;			/* 1=SNMP, 2=ServerMgrd */
	BOOL useIcmp;
	BOOL useLithiumSnmpExtensions;
	BOOL monitorProcessList;
	BOOL useNagios;
	BOOL useLom;
	BOOL isXsanClient;
	int mark;					/* 0=Production, -2=Testing, -3=OutOfService */
	
	/* Dynamic properties */
	BOOL willUseSNMP;
	BOOL isInTesting;
	BOOL isInProduction;
	BOOL isOutOfService;
	
	/* Refresh */
	BOOL recentlyRestarted;
	unsigned long refreshCount;
	unsigned long refreshTimeStamp;
	
	/* Services */
	NSMutableArray *services;
	NSMutableDictionary *serviceDict;
	
	/* Processes */
	NSMutableArray *procProfiles;
	NSMutableDictionary *procProfileDict;
	NSMutableArray *procNames;
}

- (LCDevice *) init;
- (Class) childClass;

#pragma mark "Device Entity Methods"
- (NSImage *) deviceIcon;
- (NSMutableArray *) sortedInterfaceObjects;


@property (nonatomic,copy) NSString *ipAddress;
@property (nonatomic,copy) NSString *lomIpAddress;
@property (nonatomic,copy) NSString *snmpCommunity;
@property (nonatomic, assign) int snmpVersion;
@property (nonatomic, assign) int snmpAuthMethod;
@property (nonatomic, assign) int snmpPrivacyEncryption;
@property (nonatomic,copy) NSString *snmpPrivPassword;
@property (nonatomic,copy) NSString *snmpAuthPassword;
@property (nonatomic,copy) NSString *username;
@property (nonatomic,copy) NSString *password;
@property (nonatomic,copy) NSString *lomUsername;
@property (nonatomic,copy) NSString *lomPassword;
@property (nonatomic,copy) NSString *vendor;
@property (nonatomic,copy) NSString *profile;
@property (nonatomic, assign) int refreshInterval;
@property (nonatomic, assign) int osxProtocol;
@property (nonatomic, assign) BOOL useIcmp;
@property (nonatomic, assign) BOOL useLithiumSnmpExtensions;
@property (nonatomic, assign) BOOL monitorProcessList;
@property (nonatomic, assign) BOOL useNagios;
@property (nonatomic, assign) BOOL useLom;
@property (nonatomic, assign) int mark;
@property (nonatomic, assign) BOOL isXsanClient;
@property (nonatomic, assign) BOOL recentlyRestarted;
@property (nonatomic, assign) unsigned long refreshCount;
@property (nonatomic, assign) unsigned long refreshTimeStamp;
@property (nonatomic, assign) BOOL willUseSNMP;
@property (nonatomic, assign) BOOL isInTesting;
@property (nonatomic, assign) BOOL isInProduction;
@property (nonatomic, assign) BOOL isOutOfService;


@property (readonly) NSMutableArray *services;
- (void) insertObject:(id)service inServicesAtIndex:(unsigned int)index;
- (void) removeObjectFromServicesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *serviceDict;
@property (readonly) NSMutableArray *procProfiles;
- (void) insertObject:(LCProcessProfile *)profile inProcProfilesAtIndex:(unsigned int)index;
- (void) removeObjectFromProcProfilesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *procProfileDict;
@property (readonly) NSMutableArray *procNames;

@end
