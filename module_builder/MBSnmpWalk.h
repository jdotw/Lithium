//
//  MBSnmpWalk.h
//  ModuleBuilder
//
//  Created by James Wilson on 9/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#import <Cocoa/Cocoa.h>

@class ModuleDocument;

@protocol MBSnmpWalkDelegate

@optional

- (void) scanDidFinished:(BOOL)result error:(NSString *)error;

@end


@interface MBSnmpWalk : NSObject 
{
	/* Document */
	ModuleDocument *document;
	id <MBSnmpWalkDelegate> delegate;

	/* Walk properties */
	NSString *community;
	NSString *ip;
	NSString *startOid;
	long snmpVersion;
	BOOL bulkwalk;
	BOOL canUseBulkWalk;
	BOOL replaceExistingScan;
	int timeoutSeconds;
	int retries;
	
	/* SNMP Session */
	struct snmp_session *session;
	BOOL sessionOpen;
	BOOL walkInProgress;
	BOOL shouldTearDown;
	int oidsReceived;
	NSTimer *timeoutTimer;
	CFSocketRef sockref;
	oid name[MAX_OID_LEN];
	size_t name_length;
	oid root [MAX_OID_LEN];
	size_t root_length;
	size_t pdusReceived;
}

#pragma mark Socket API
- (void) resetSocketTimeout;
- (void) socketCallback;

#pragma mark Timeout Handling
- (void) timeoutCallback:(NSTimer *)timer;

#pragma mark SNMP API
- (void) processPDU:(struct snmp_pdu *)pdu reqid:(int)reqid operation:(int)operation;
- (void) startWalk;
- (void) getNext;
- (void) cancel;
- (void) teardown;

#pragma mark Properties
@property (nonatomic, assign) ModuleDocument *document;
@property (nonatomic, assign) id <MBSnmpWalkDelegate> delegate;
@property (nonatomic, copy) NSString *community;
@property (nonatomic, copy) NSString *ip;
@property (nonatomic, copy) NSString *startOid;
@property (nonatomic, assign) BOOL replaceExistingScan;
@property (nonatomic, assign) long snmpVersion;
@property (nonatomic, assign) BOOL bulkwalk;
@property (nonatomic, assign) BOOL canUseBulkWalk;
@property (nonatomic, assign) int timeoutSeconds;
@property (nonatomic, assign) int retries;
@property (nonatomic, assign) BOOL sessionOpen;
@property (nonatomic, assign) BOOL walkInProgress;
@property (nonatomic, assign) int oidsReceived;


@end
