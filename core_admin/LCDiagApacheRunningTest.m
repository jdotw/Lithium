//
//  LCDiagApacheRunningTest.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagApacheRunningTest.h"

#import "LCDiagWebServerName.h"

#import "LCProcessList.h"

@implementation LCDiagApacheRunningTest

- (id) init
{
	[super init];
	
	[self insertObject:[[LCDiagWebServerName alloc] init] inChildTestsAtIndex:[childTests count]];
	
	return self;
}

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if ([[LCProcessList masterList] apacheRunning])
	{ [self testPassed]; }
	else
	{
		/* Core is not running */
		[self testFailed];
	}
}

- (NSString *) testDescription
{
	return @"Check Client Service (clientd) is Running";
}

@end
