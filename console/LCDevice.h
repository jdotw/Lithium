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


@property (copy) NSString *ipAddress;
@property (copy) NSString *lomIpAddress;
@property (copy) NSString *snmpCommunity;
@property (assign) int snmpVersion;
@property (assign) int snmpAuthMethod;
@property (assign) int snmpPrivacyEncryption;
@property (copy) NSString *snmpPrivPassword;
@property (copy) NSString *snmpAuthPassword;
@property (copy) NSString *username;
@property (copy) NSString *password;
@property (copy) NSString *lomUsername;
@property (copy) NSString *lomPassword;
@property (copy) NSString *vendor;
@property (copy) NSString *profile;
@property (assign) int refreshInterval;
@property (assign) int osxProtocol;
@property (assign) BOOL useIcmp;
@property (assign) BOOL useLithiumSnmpExtensions;
@property (assign) BOOL monitorProcessList;
@property (assign) BOOL useNagios;
@property (assign) BOOL useLom;
@property (assign) int mark;
@property (assign) BOOL isXsanClient;
@property (assign) BOOL recentlyRestarted;
@property (assign) unsigned long refreshCount;
@property (assign) unsigned long refreshTimeStamp;
@property (assign) BOOL willUseSNMP;
@property (assign) BOOL isInTesting;
@property (assign) BOOL isInProduction;
@property (assign) BOOL isOutOfService;


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
