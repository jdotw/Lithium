//
//  LCDiagSQLCheckCustomers.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagSQLCheckCustomers.h"
#import "LCConfigController.h"
#import "LCDiagSQLCustomerLogin.h"

#include <libpq-fe.h>

@implementation LCDiagSQLCheckCustomers

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	/* Get Config Controller */
	LCConfigController *configController = [LCConfigController masterController];

	/* Remove previous tests */
	[self willChangeValueForKey:@"childTests"];
	[childTests removeAllObjects];
	[self didChangeValueForKey:@"childTests"];	
	
	/* Connect */
	PGconn *conn;
	conn = PQsetdbLogin([[configController dbHostname] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPort] cStringUsingEncoding:NSUTF8StringEncoding], NULL, NULL, "lithium", [[configController dbUsername] cStringUsingEncoding:NSUTF8StringEncoding], [[configController dbPassword] cStringUsingEncoding:NSUTF8StringEncoding]);
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self testFailed];
		return;
	}
	
	/* Query customers */
	PGresult *res = PQexec (conn, "SELECT name FROM customers");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		[self testFailed];
		return;
	}
	
	/* Loop through customer */
	int i;
	int numRows = PQntuples(res);
	for (i=0; i < numRows; i++)
	{
		[self insertObject:[[LCDiagSQLCustomerLogin alloc] initForCustomerNamed:[NSString stringWithFormat:@"%s", PQgetvalue(res,i,0)]]
	   inChildTestsAtIndex:[childTests count]];
	}

	/* Check row count */
	if (numRows > 1)
	{
		[self testWarning];
	}
	else 
	{ 
		[self testPassed];
	}	
	
	/* Close */
//	PQfinish(res);
}

- (NSString *) testDescription
{
	return @"Check Configured Customer in SQL Database";
}

@end
