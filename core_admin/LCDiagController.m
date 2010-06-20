//
//  LCDiagController.m
//  LCAdminTools
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagController.h"


@implementation LCDiagController

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	/* Create test marshall */
	[self setTestMarshall:[[[LCDiagTestMarshall alloc] initWithController:self] autorelease]];
}

#pragma mark "Run Diagnostic"

- (IBAction) runDiagnosticClicked:(id)sender
{
	[self setTestInProgress:YES];
	[testMarshall performTest:self];
}

- (IBAction) emailResultClicked:(id)sender
{
	
}

#pragma mark "Test Delegate"

- (void) childTestFinished:(LCDiagTest *)childTest
{
	/* Marshall finished */
	[self setTestInProgress:NO];
}

- (NSMutableArray *) liveTests
{ return nil; }

- (void) setLiveTests:(NSMutableArray *)array
{
}

- (void) insertObject:(LCDiagTest *)test inLiveTestsAtIndex:(unsigned int)index
{  }

- (void) removeObjectFromLiveTestsAtIndex:(unsigned int)index
{  }

#pragma mark "Expanding"

- (void) expandAll
{
	int i;
	for (i=0; i < [testOutlineView numberOfRows]; i++)
	{ 
		[testOutlineView expandItem:[testOutlineView itemAtRow:i]]; 
	}
	[testOutlineView scrollRowToVisible:[testOutlineView numberOfRows]-1];
}

#pragma mark "Accessors"

- (LCDiagTestMarshall *) testMarshall
{ return testMarshall; }

- (void) setTestMarshall:(LCDiagTestMarshall *)marshall
{
	[testMarshall release];
	testMarshall = [marshall retain];
}

- (BOOL) testInProgress
{ return testInProgress; }

- (void) setTestInProgress:(BOOL)flag
{ testInProgress = flag; }


@end
