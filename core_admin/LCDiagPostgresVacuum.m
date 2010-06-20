//
//  LCDiagPostgresVacuum.m
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagPostgresVacuum.h"

#import "LCPerformController.h"

@implementation LCDiagPostgresVacuum

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if ([[LCPerformController masterController] vacuumInProgress])
	{ [self testWarning]; }
	else
	{ [self testPassed]; }
}

- (NSString *) testDescription
{ return @"Check for PostgreSQL VACUUM operation(s) in progress"; }


@end
