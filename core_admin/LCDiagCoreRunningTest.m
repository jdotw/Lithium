//
//  LCDiagCoreRunningTest.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCoreRunningTest.h"
#import "LCProcessList.h"
#import "LCDiagCPUUsage.h"
#import "LCDiagMemUsage.h"
#import "LCDiagCustomerList.h"

@implementation LCDiagCoreRunningTest

#pragma mark "Test"

- (id) init
{
	[super init];
	[self insertObject:[[LCDiagCPUUsage alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagMemUsage alloc] init] inChildTestsAtIndex:[childTests count]];	
	[self insertObject:[[LCDiagCustomerList alloc] init] inChildTestsAtIndex:[childTests count]];
	return self;
}

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if ([[LCProcessList masterList] lithiumRunning])
	{ [self testPassed]; }
	else
	{
		/* Core is not running */
		[self testFailed];
	}
}

- (NSString *) testDescription
{
	return @"Check Lithium Core Processes are Running";
}

@end
