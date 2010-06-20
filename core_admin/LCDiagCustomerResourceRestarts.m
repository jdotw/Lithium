//
//  LCDiagCustomerResourceRestarts.m
//  LCAdminTools
//
//  Created by James Wilson on 4/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCustomerResourceRestarts.h"


@implementation LCDiagCustomerResourceRestarts

- (void) validateResources
{
	int highestCount = 0;
	NSEnumerator *resEnum = [resources objectEnumerator];
	NSMutableDictionary *res;
	while (res=[resEnum nextObject])
	{
		if ([[res objectForKey:@"restart_count"] intValue] > highestCount)
		{ highestCount = [[res objectForKey:@"restart_count"] intValue]; }
	}
	
	if (highestCount > 20)
	{ 
		[self testFailed]; 
	}
	else if (highestCount > 0)
	{ 
		[self testWarning]; 
	}
	else
	{ [self testPassed]; }
}

- (NSString *) testDescription
{
	return @"Check for monitoring process crashes/restarts";
}


@end
