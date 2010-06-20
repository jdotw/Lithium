//
//  LCDiagPostgresProcCount.m
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagPostgresProcCount.h"

#import "LCPerformController.h"

@implementation LCDiagPostgresProcCount

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	LCPerformanceGroup *group = [[LCPerformController masterController] postgresGroup];
	
	if ([[group items] count] > 25)
	{
		[self testWarning];
	}
	else if ([[group items] count] < 2)
	{
		[self testFailed];
	}
	else
	{
		[self testPassed];
	}	
}

- (NSString *) testDescription
{ return @"Check number of PostgreSQL processes"; }

@end
