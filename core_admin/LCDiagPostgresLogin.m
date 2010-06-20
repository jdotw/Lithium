//
//  LCDiagPostgresLogin.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagPostgresLogin.h"
#import "LCConfigController.h"
#import "LCDiagSQLCheckCustomers.h"

#include <libpq-fe.h>

@implementation LCDiagPostgresLogin

#pragma mark "Initialisation"

- (id) init
{
	[super init];
	[self insertObject:[[LCDiagSQLCheckCustomers alloc] init] inChildTestsAtIndex:[childTests count]];	
	return self;
}

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	/* Get Config Controller */
	LCConfigController *configController = [LCConfigController masterController];
	
	/* Connect */
	PGconn *conn;
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (PQstatus(conn) == CONNECTION_OK)
	{
		[self testPassed];
		PQfinish (conn);
	}
	else
	{
		[self testFailed];
	}	
}

- (NSString *) testDescription
{
	return @"Attempt to login to PostgreSQL Database";
}

@end
