//
//  MBCoreDeployment.h
//  ModuleBuilder
//
//  Created by James Wilson on 22/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MBCoreCustomer.h"

@interface MBCoreDeployment : NSObject 
{
	NSString *ipAddress;

	NSMutableArray *customers;
	NSMutableDictionary *customerDict;
	MBCoreCustomer *selectedCustomer; 
	
	NSString *username;
	NSString *password;
	
	NSString *status;
	int statusInteger;
	NSImage *statusIcon;
	
	BOOL selected;
	BOOL newlyAdded;
	
	/* URL Download */
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
	NSString *curXMLElement;
	NSMutableString *curXMLString;
	NSMutableDictionary *curXMLDictionary;
	BOOL refreshInProgress;	
	BOOL errorEncountered;
	BOOL disabled;
}

#pragma mark "Refresh Deployment"
- (void) refreshDeployment;

#pragma mark "Properties"
@property (copy) NSString *ipAddress;
@property (copy) NSMutableArray *customers;
@property (copy) NSMutableDictionary *customerDict;
- (void) insertObject:(MBCoreCustomer *)obj inCustomersAtIndex:(unsigned int)index;
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index;
@property (retain) MBCoreCustomer *selectedCustomer;
@property (copy) NSString *username;
@property (copy) NSString *password;
@property (copy) NSString *status;
@property (assign) int statusInteger;
@property (copy) NSImage *statusIcon;
@property (assign) BOOL refreshInProgress;
@property (assign) BOOL errorEncountered;
@property (assign) BOOL selected;
@property (assign) BOOL newlyAdded;

@end
