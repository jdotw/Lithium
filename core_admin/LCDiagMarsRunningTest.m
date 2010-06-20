//
//  LCDiagMarsRunningTest.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagMarsRunningTest.h"

#import "LCProcessList.h"

@implementation LCDiagMarsRunningTest

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if ([[LCProcessList masterList] rrdtoolRunning])
	{ [self testPassed]; }
	else
	{
		/* Core is not running */
		[self testFailed];
	}
}

- (NSString *) testDescription
{
	return @"Check Metric Recording Processes are Running";
}

@end
