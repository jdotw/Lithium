//
//  LCDiagMemUsage.m
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagMemUsage.h"

#import "LCPerformanceGroup.h"
#import "LCPerformanceItem.h"
#import "LCPerformanceProcess.h"
#import "LCPerformController.h"

@implementation LCDiagMemUsage

- (void) sampleUsage
{
	runCount++;
	
	[self setResultString:[NSString stringWithFormat:@"Sampling (%i/10)", runCount]];
	
	LCPerformanceGroup *procGroup = [[LCPerformController masterController] lithiumGroup];
	
	/* Mem Average */
	groupMemAverage = groupMemAverage * (runCount-1);
	groupMemAverage = groupMemAverage + ([[procGroup rSize] floatValue] / (float) [[procGroup items] count]);
	groupMemAverage = groupMemAverage / (float) runCount;
	
	/* High processes */
	NSEnumerator *itemEnum = [[procGroup items] objectEnumerator];
	LCPerformanceItem *item;
	NSMutableArray *newHighList = [NSMutableArray array];
	while (item=[itemEnum nextObject])
	{
		if ([[item rSize] floatValue] > (100.0 * 1024.0))
		{
			[newHighList addObject:item];
		}
	}
	NSEnumerator *highEnum = [newHighList objectEnumerator];
	while (item=[highEnum nextObject])
	{
		if (runCount != 1 && ![highList containsObject:item])
		{ [newHighList removeObject:item]; }
	}
	[highList release];
	highList = [newHighList retain];
	
	/* Check Round Count */
	if (runCount == 10)
	{
		if (groupMemAverage > (20.0 * 1024.0))
		{ [self testWarning]; }
		else if ([highList count] > 0)
		{ [self testFailed]; }
		else 
		{ [self testPassed]; }
		
		[refreshTimer invalidate];
		[refreshTimer release];
		refreshTimer = nil;
	}
}

#pragma mark "Test"

- (void) performTest:(id)theDelegate
{
	[super performTest:theDelegate];
	
	if (refreshTimer)
	{ 
		[refreshTimer invalidate];
		[refreshTimer release];
	}
	
	groupMemAverage = 0.0;
	runCount = 0;
	
	refreshTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0
													 target:self
												   selector:@selector(sampleUsage)
												   userInfo:nil
													repeats:YES] retain];
	[self sampleUsage];
}

- (NSString *) testDescription
{
	return @"Check Lithium Core RAM Usage";
}


@end
