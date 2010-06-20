//
//  LCDiagSQLCustomerLogin.m
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagSQLCustomerLogin.h"

#import "LCConfigController.h"
#import "LCDiagSQLDeviceTable.h"
#import "LCDiagSQLSiteTable.h"

#include <libpq-fe.h>

@implementation LCDiagSQLCustomerLogin

#pragma mark "Initialisation"

- (LCDiagSQLCustomerLogin *) initForCustomerNamed:(NSString *)initName
{
	[super init];
	
	[self setCustomerName:(NSString *)initName];
	
	[self insertObject:[[LCDiagSQLDeviceTable alloc] initForCustomerNamed:initName]
   inChildTestsAtIndex:[childTests count]];
	[self insertObject:[[LCDiagSQLSiteTable alloc] initForCustomerNamed:initName]
   inChildTestsAtIndex:[childTests count]];

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
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, 
						[[NSString stringWithFormat:@"customer_%@", [self customerName]] cStringUsingEncoding:NSUTF8StringEncoding], 
						[[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
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
	return [NSString stringWithFormat:@"Attempt to login to '%@' database", [self customerName]];
}

- (NSString *) customerName
{ return customerName; }

- (void) setCustomerName:(NSString *)string
{
	[customerName release];
	customerName = [string retain];
}

@end
