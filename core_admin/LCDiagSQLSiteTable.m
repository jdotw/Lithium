//
//  LCDiagSQLSiteTable.m
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagSQLSiteTable.h"

#import "LCConfigController.h"

#include <libpq-fe.h>

@implementation LCDiagSQLSiteTable

#pragma mark "Initialisation"

- (LCDiagSQLSiteTable *) initForCustomerNamed:(NSString *)initName
{
	[super init];
	
	[self setCustomerName:initName];
	
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
	if (PQstatus(conn) == CONNECTION_BAD)
	{
		[self testFailed];
		return;
	}	
	
	/* Query customers */
	PGresult *res = PQexec (conn, "SELECT name, descr FROM sites");
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
		NSString *name = [NSString stringWithFormat:@"%s", PQgetvalue(res,i,0)];
		NSString *desc = [NSString stringWithFormat:@"%s", PQgetvalue(res,i,1)];
		
		if (!name || [name length] < 1)
		{ 
			[self testFailed];
			return;
		}
		else if (!desc || [desc length] < 1)
		{
			[self testFailed];
			return;
		}
	}
	if (numRows == 0)
	{
		[self testWarning];
		return;
	}
	
	[self testPassed];
}

- (NSString *) testDescription
{
	return [NSString stringWithFormat:@"Checking 'sites' table", [self customerName]];
}

- (NSString *) customerName
{ return customerName; }

- (void) setCustomerName:(NSString *)string
{
	[customerName release];
	customerName = [string retain];
}

@end
