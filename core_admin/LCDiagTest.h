//
//  LCDiagTest.h
//  LCAdminTools
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface LCDiagTest : NSObject 
{
	NSString *resultString;
	NSImage *resultIcon;
	
	NSMutableArray *childTests;
	NSMutableArray *liveTests;
	
	id delegate;
}

#pragma mark "Constructor"
- (LCDiagTest *) init;

#pragma mark "Test"
- (void) performTest:(id)testDelegate;
- (void) finishedTest;
- (void) childTestFinished:(LCDiagTest *)childTest;

#pragma mark "Accessors"
- (NSString *) resultString;
- (void) setResultString:(NSString *)string;
- (NSImage *) resultIcon;
- (void) setResultIcon:(NSImage *)image;
- (NSMutableArray *) childTests;
- (void) insertObject:(LCDiagTest *)test inChildTestsAtIndex:(unsigned int)index;
- (void) removeObjectFromChildTestsAtIndex:(unsigned int)index;
- (NSMutableArray *) liveTests;
- (void) setLiveTests:(NSMutableArray *)array;
- (void) insertObject:(LCDiagTest *)test inLiveTestsAtIndex:(unsigned int)index;
- (void) removeObjectFromLiveTestsAtIndex:(unsigned int)index;
- (id) delegate;
- (void) setDelegate:(id)newDelegate;
- (id) controller;

#pragma mark "Helpers"
- (void) testFailed;
- (void) testPassed;
- (void) testWarning;


@end
