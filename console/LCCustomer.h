//
//  LCCustomer.h
//  Lithium Console
//
//  Created by James Wilson on 20/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCIncidentList.h"
#import "LCCaseList.h"
#import "LCUserList.h"
#import "LCVendorList.h"
#import "LCServiceList.h"
#import "LCProcessProfileList.h"
#import "LCXsanList.h"
#import "LCLunList.h"
#import "LCDocumentList.h"
#import "LCGroupTree.h"

@interface LCCustomer : LCEntity 
{
	/* Customer Properties */
	NSString *cluster;
	NSString *node;
	NSString *url;
	NSString *coreVersion;
	NSString *inductionVersion;
	int licenseType;
	BOOL limitedLicense;
	
	/* State */
	BOOL disabled;
	
	/* Lists */
	LCCaseList *openCasesList;
	LCIncidentList *activeIncidentsList;
	LCVendorList *vendorList;
	LCUserList *userList;
	LCServiceList *serviceList;
	LCProcessProfileList *processProfileList;
	LCXsanList *xsanList;
	LCLunList *lunList;
	LCDocumentList *documentList;
	LCGroupTree *groupTree;
	
	/* Refresh Timers */
	NSTimer *openCasesRefreshTimer;
	NSTimer *activeIncidentsRefreshTimer;
	
	/* Credentials */
	NSString *username;
	NSString *password;
	int userAuthLevel;
	BOOL userIsAdmin;
	BOOL userIsNormal;
	BOOL userIsReadOnly;
	
	/* Global Entity Array */
	NSMutableArray *globalEntityArray;
	
	/* Setup Controller */
	id setupController;
	
	/* Persistent Windows */
	NSTimer *thawTimer;
	NSMutableArray *persistentWindows;
	
	/* Auto-Refresh */
	NSTimer *customerRefreshTimer;
}

#define LIC_NONE 0
#define LIC_PAID 1
#define LIC_DEMO_VALID 2
#define LIC_DEMO_EXPIRED 3
#define LIC_NFR_VALID 4
#define LIC_NFR_EXPIRED 5
#define LIC_FREE 6

#pragma mark "Initialisation"
+ (id) customerWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initBaseURL;
- (id) initWithName:(NSString *)initName cluster:(NSString *)initCluster node:(NSString *)initNode url:(NSString *)initBaseURL;
- (void) dealloc;

#pragma mark "Refresh Manipuilation Methods"
- (void) setOpenCasesAutoRefresh:(BOOL)flag;
- (void) setActiveIncidentsAutoRefresh:(BOOL)flag;

- (NSTimer *) openCasesRefreshTimer;
- (NSTimer *) activeIncidentsRefreshTimer;

#pragma mark "Global Entity Array"
- (NSMutableArray *) globalEntityArray;
- (void) insertObject:(LCEntity *)entity inGlobalEntityArrayAtIndex:(unsigned int)index;
- (void) removeObjectFromGlobalEntityArrayAtIndex:(unsigned int)index;

#pragma mark "User Auth Levels"
@property (assign) int userAuthLevel;
@property (assign) BOOL userIsAdmin;
@property (assign) BOOL userIsNormal;
@property (assign) BOOL userIsReadOnly;

#pragma mark "Accessor Methods"
- (LCCaseList *) openCasesList;
- (LCIncidentList *) activeIncidentsList;
- (LCUserList *) userList;	
- (LCVendorList *) vendorList;
- (LCServiceList *) serviceList;
- (LCXsanList *) xsanList;
- (LCLunList *) lunList;
- (NSString *) username;
- (void) setUsername:(NSString *)string;
- (NSString *) password;
- (void) setPassword:(NSString *)string;
- (id) setupController;
- (void) setSetupController:(id)controller;

@property (retain) NSString *cluster;
@property (retain) NSString *node;
@property (retain) NSString *url;
@property (retain) NSString *coreVersion;
@property (retain) NSString *inductionVersion;
@property int licenseType;
@property (assign) BOOL limitedLicense;
@property BOOL disabled;
@property (readonly) LCCaseList *openCasesList;
@property (retain) NSTimer *openCasesRefreshTimer;
@property (readonly) LCIncidentList *activeIncidentsList;
@property (retain) NSTimer *activeIncidentsRefreshTimer;
@property (readonly) LCVendorList *vendorList;
@property (readonly) LCUserList *userList;
@property (readonly) LCServiceList *serviceList;
@property (readonly) LCProcessProfileList *processProfileList;
@property (readonly) LCXsanList *xsanList;
@property (readonly) LCLunList *lunList;
@property (readonly) LCDocumentList *documentList;
@property (readonly) LCGroupTree *groupTree;
@property (copy) NSString *username;
@property (copy) NSString *password;
@property (readonly) NSMutableArray *globalEntityArray;
@property (assign) id setupController;
@property (retain) NSTimer *thawTimer;
@property (readonly) NSMutableArray *persistentWindows;
@property (retain) NSTimer *customerRefreshTimer;


@end
