//
//  LCDiagTest.m
//  LCAdminTools
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagTest.h"

#import "LCDiagController.h"

@implementation LCDiagTest

#pragma mark "Constructor"

- (LCDiagTest *) init
{
	[super init];
	
	childTests = [[NSMutableArray array] retain];
	
	return self;
}

#pragma mark "Test"

- (void) performTest:(id)testDelegate
{
	/* Reset Live Tests */
	[self setLiveTests:[NSMutableArray array]];
	
	/* Set delegate */
	[self setDelegate:testDelegate];
	[delegate insertObject:self inLiveTestsAtIndex:[[delegate liveTests] count]];
	[(LCDiagController *)[self controller] expandAll];
	
	/* Set status */
	[self setResultString:@"Checking..."];
	[self setResultIcon:nil];
}

- (void) finishedTest
{
	/* Check for child tests */
	if ([childTests count] > 0)
	{
		/* Has children */
		[[childTests objectAtIndex:0] performTest:self];
	}
	else
	{
		/* Has no children */
		[(LCDiagTest *) delegate childTestFinished:self];
	}
}

- (void) childTestFinished:(LCDiagTest *)childTest
{
	int curIndex = [childTests indexOfObject:childTest];
	if ((curIndex + 1) == [childTests count])
	{
		/* All child tests completed */
		[delegate childTestFinished:self];
	}
	else
	{
		/* Fire off next child test */
		LCDiagTest *nextChild = [childTests objectAtIndex:curIndex+1];
		[nextChild performTest:self];
	}
}

#pragma mark "Accessors"

- (NSString *) testDescription
{ return nil; }

- (NSString *) resultString
{ return resultString; }

- (void) setResultString:(NSString *)string
{
	[resultString release];
	resultString = [string retain];
}

- (NSImage *) resultIcon
{ return resultIcon; }

- (void) setResultIcon:(NSImage *)image
{
	[resultIcon release];
	resultIcon = [image retain];
}

- (NSMutableArray *) childTests
{ return childTests; }

- (void) insertObject:(LCDiagTest *)test inChildTestsAtIndex:(unsigned int)index
{ [childTests insertObject:test atIndex:index]; }

- (void) removeObjectFromChildTestsAtIndex:(unsigned int)index
{ [childTests removeObjectAtIndex:index]; }

- (NSMutableArray *) liveTests
{ return liveTests; }

- (void) setLiveTests:(NSMutableArray *)array
{
	[liveTests release];
	liveTests = [array retain];
}

- (void) insertObject:(LCDiagTest *)test inLiveTestsAtIndex:(unsigned int)index
{ [liveTests insertObject:test atIndex:index]; }

- (void) removeObjectFromLiveTestsAtIndex:(unsigned int)index
{ [liveTests removeObjectAtIndex:index]; }

- (id) delegate
{ return delegate; }

- (void) setDelegate:(id)newDelegate
{ delegate = newDelegate; }

- (id) controller
{ return [delegate controller]; }

#pragma mark "Helpers"

- (void) testFailed
{
	[self setResultString:@"FAILED"];
	[self setResultIcon:[NSImage imageNamed:@"RedDot.tiff"]];
	[self finishedTest];
}

- (void) testPassed 
{
	[self setResultString:@"OK"];
	[self setResultIcon:[NSImage imageNamed:@"GreenDot.tiff"]];
	[self finishedTest];
}

- (void) testWarning
{
	[self setResultString:@"WARNING"];
	[self setResultIcon:[NSImage imageNamed:@"YellowDot.tiff"]];
	[self finishedTest];
}

@end
 