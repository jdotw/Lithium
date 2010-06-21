//
//  LCCase.h
//  Lithium Console
//
//  Created by James Wilson on 25/07/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"
#import "LCCustomer.h"
#import "LCCaseEntityList.h"
#import "LCCaseLogEntryList.h"
#import "LCXMLObject.h"

@interface LCCase : LCXMLObject 
{
	/* Related Objects */
	LCCustomer *customer;
	
	/* Properties */
	unsigned long caseID;
	int state;
	NSString *stateString;
	NSString *headline;
	NSString *requester;
	NSString *owner;
	long openDateSeconds;
	NSDate *openDate;
	long closeDateSeconds;
	NSDate *closeDate;
	NSString *openDateShortString;
	NSString *closeDateShortString;
	BOOL isClosed;
	NSString *fullIDString;
	
	/* Lists */
	LCCaseEntityList *entityList;
	LCCaseLogEntryList *logEntryList;
	LCCaseLogEntry *lastLogEntry;

	/* Auto Refresh */
	BOOL entityListAutoRefresh;
	BOOL logEntryListAutoRefresh;	
	NSTimer *entityListRefreshTimer;
	NSTimer *logEntryListRefreshTimer;
	
	/* Op variables */
	id openDelegate;
	id closeDelegate;
	id reOpenDelegate;
	LCXMLRequest *openXMLReq;
	LCXMLRequest *closeXMLReq;
	LCXMLRequest *updateXMLReq;
	LCXMLRequest *reOpenXMLReq;
}

#pragma mark "Case Manipulation Methods"
- (void) openCaseWithInitialLogEntry:(NSString *)logentry delegate:(id)delegate;
- (void) closeCaseWithFinalLogEntry:(NSString *)logentry delegate:(id)delegate;
- (void) updateCase;
- (void) reOpenCaseWithLogEntry:(NSString *)logentry delegate:(id)delegate;

#pragma mark "Refresh Methods"
@property (nonatomic, assign) BOOL entityListAutoRefresh;
@property (nonatomic,retain) NSTimer *entityListRefreshTimer;
@property (nonatomic, assign) BOOL logEntryListAutoRefresh;
@property (nonatomic,retain) NSTimer *logEntryListRefreshTimer;

#pragma mark "View Controller Methods"
- (id) viewController;

#pragma mark "Properties"
@property (nonatomic,retain) LCCustomer *customer;
@property (readonly) LCCaseEntityList *entityList;
@property (readonly) LCCaseLogEntryList *logEntryList;
@property (nonatomic, assign) unsigned long caseID;
@property (nonatomic,copy) NSString *requester;
@property (nonatomic,copy) NSString *owner;
@property (nonatomic,copy) NSString *headline;
@property (nonatomic, assign) int state;
@property (nonatomic,copy) NSString *stateString;
@property (nonatomic, assign) long openDateSeconds;
@property (nonatomic,copy) NSDate *openDate;
@property (nonatomic, assign) long closeDateSeconds;
@property (nonatomic,copy) NSDate *closeDate;
@property (nonatomic,copy) NSString *openDateShortString;
@property (nonatomic,copy) NSString *closeDateShortString;
@property (nonatomic, assign) BOOL isClosed;
@property (nonatomic,copy) NSString *fullIDString;
@property (nonatomic,retain) LCCaseLogEntry *lastLogEntry;

#define CASESTATE_OPEN 1
#define CASESTATE_OPEN_STR @"1"

@end
