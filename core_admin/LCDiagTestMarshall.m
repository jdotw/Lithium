//
//  LCDiagTestMarshall.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagTestMarshall.h"
#import "LCDiagLoadTest.h"
#import "LCDiagCoreRunningTest.h"
#import "LCDiagPostgresRunningTest.h"
#import "LCDiagMarsRunningTest.h"
#import "LCDiagApacheRunningTest.h"
#import "LCDiagCPUUsage.h"
#import "LCDiagMemUsage.h"
#import "LCDiagCrashReporter.h"
#import "LCDiagPostgresLogin.h"
#import "LCDiagSQLCheckCustomers.h"

@implementation LCDiagTestMarshall

#pragma mark "Constructors"

- (LCDiagTestMarshall *) initWithController:(id)initController
{
	[super init];

	/* Set Controller */
	[self setController:initController];
	
	/* 
	 * Create Primary Tests
	 */

	[self insertObject:[[LCDiagCrashReporter alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagLoadTest alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagPostgresRunningTest alloc] init] inChildTestsAtIndex:[childTests count]];
	[self insertObject:[[LCDiagMarsRunningTest alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagApacheRunningTest alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagCoreRunningTest alloc] init] inChildTestsAtIndex:[childTests count]];	
	
	return self;
}

#pragma mark "Dummy Test"

- (void) performTest:(id)testDelegate
{
	[super performTest:testDelegate];
	[self finishedTest];
}

#pragma mark "Accessors"

- (id) controller
{ return controller; }

- (void) setController:(id)newController
{ controller = newController; }

@end
