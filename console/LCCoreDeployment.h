//
//  LCCoreDeployment.h
//  Lithium Console
//
//  Created by James Wilson on 23/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import <SystemConfiguration/SystemConfiguration.h>
#import <SystemConfiguration/SCNetworkReachability.h>

@interface LCCoreDeployment : NSObject 
{
	/* Properties -- Serialised */
	NSMutableDictionary *properties;
	
	/* Run-Time Data */
	NSString *version;
	NSString *status;
	int statusInteger;
	NSImage *statusIcon;
	BOOL isDynamic;
	NSMutableArray *customers;
	
	/* Reachability */
	SCNetworkReachabilityRef reachabilityRef;
	BOOL reachable;
	
	/* URL Download */
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSMutableString *curXMLString;
	NSMutableDictionary *curXMLDictionary;
	BOOL refreshInProgress;	
	BOOL errorEncountered;
	BOOL disabled;
	BOOL testMode;
}

#pragma mark "Test Deployment"
- (void) refreshDeployment;
- (void) testDeployment;

#pragma mark "Alert"
- (void) displayAlert;

#pragma mark "Reachability Methods"
@property (nonatomic, assign) BOOL reachable;

#pragma mark "General Properties"
@property (nonatomic,copy) NSMutableDictionary *properties;
@property (readonly) NSString *url;
@property (nonatomic,copy) NSString *address;
@property (nonatomic, assign) int port;
@property (nonatomic, assign) BOOL useSSL;
@property (nonatomic,copy) NSString *version;
@property (nonatomic,copy) NSString *status;
@property (nonatomic, assign) BOOL isDynamic;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic, assign) int statusInteger;
@property (nonatomic,copy) NSImage *statusIcon;
@property (nonatomic, assign) BOOL errorEncountered;
@property (readonly) BOOL isCustomer;
@property (readonly) NSArray *customers;
@property (nonatomic, assign) BOOL disabled;
@property (readonly) NSImage *discoveredIcon;
@property (readonly) BOOL isDeployment;
- (void) insertObject:(LCCustomer *)customer inCustomersAtIndex:(unsigned int)index;
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index;

@end
