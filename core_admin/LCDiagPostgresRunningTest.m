//
//  LCDiagPostgresRunningTest.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagPostgresRunningTest.h"
#import "LCDiagPostgresLogin.h"
#import "LCDiagPostgresProcCount.h"
#import "LCDiagPostgresVacuum.h"

#import "LCProcessList.h"

@implementation LCDiagPostgresRunningTest

- (id) init
{
	[super init];
	[self insertObject:[[LCDiagPostgresProcCount alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagPostgresVacuum alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagPostgresLogin alloc] init] inChildTestsAtIndex:[childTests count]];	
	return self;
}

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if ([[LCProcessList masterList] postgresRunning])
	{ [self testPassed]; }
	else
	{
		/* Core is not running */
		[self testFailed];
	}
}

- (NSString *) testDescription
{ return @"Check PostgreSQL Database is Running"; }

@end
