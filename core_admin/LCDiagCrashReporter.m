//
//  LCDiagCrashReporter.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCrashReporter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

@implementation LCDiagCrashReporter

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	struct stat sb;
	if (stat("/Library/Logs/CrashReporter/lithium.crash.log", &sb) == 0)
	{
		struct timeval now;
		
		gettimeofday (&now, NULL);
		
		if (sb.st_mtimespec.tv_sec > (now.tv_sec - (86400 * 2)))
		{ [self testWarning]; }
		else
		{ [self testPassed]; }
	}
	else
	{ 
		[self testPassed]; 
		[self setResultString:@"No crash log present"];
	}
}

- (NSString *) testDescription
{ return @"Checking for recent Software Crashes within 48 hours"; }

@end
