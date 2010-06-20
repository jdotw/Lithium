//
//  LCDiagLoadTest.m
//  LCAdminTools
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagLoadTest.h"


@implementation LCDiagLoadTest

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	NSTask *pipeTask = [[NSTask alloc] init];
    NSPipe *newPipe = [NSPipe pipe];
    NSFileHandle *readHandle = [newPipe fileHandleForReading];
    NSData *inData = nil;
	
    [pipeTask setStandardOutput:newPipe]; 
    [pipeTask setLaunchPath:@"/usr/bin/uptime"];
    [pipeTask launch];
	
    while ((inData = [readHandle availableData]) && [inData length]) 
	{
		NSString *string = [[[NSString alloc] initWithData:inData encoding:NSUTF8StringEncoding] autorelease];
		NSArray *components = [string componentsSeparatedByString:@" "];
		
		float one = [[components objectAtIndex:[components count]-3] floatValue];
		float five = [[components objectAtIndex:[components count]-2] floatValue];
		float fifteen = [[components objectAtIndex:[components count]-1] floatValue];
		
		if (fifteen > 9.0)
		{
			[self testWarning];
		}
		else if (five > 10.0)
		{
			[self testWarning];
		}
		else if (one > 15.0)
		{
			[self testWarning];
		}
		else
		{ [self testPassed]; }
    }
	
    [pipeTask release];	
}

- (NSString *) testDescription
{ return @"Check System Load Averages"; }

@end
