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
@property (assign) BOOL reachable;

#pragma mark "General Properties"
@property (copy) NSMutableDictionary *properties;
@property (readonly) NSString *url;
@property (copy) NSString *address;
@property (assign) int port;
@property (assign) BOOL useSSL;
@property (copy) NSString *version;
@property (copy) NSString *status;
@property (assign) BOOL isDynamic;
@property (assign) BOOL refreshInProgress;
@property (assign) int statusInteger;
@property (copy) NSImage *statusIcon;
@property (assign) BOOL errorEncountered;
@property (readonly) BOOL isCustomer;
@property (readonly) NSArray *customers;
@property (assign) BOOL disabled;
@property (readonly) NSImage *discoveredIcon;
@property (readonly) BOOL isDeployment;
- (void) insertObject:(LCCustomer *)customer inCustomersAtIndex:(unsigned int)index;
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index;

@end
