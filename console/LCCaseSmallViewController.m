//
//  LCCaseSmallViewController.m
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseSmallViewController.h"


@implementation LCCaseSmallViewController

#pragma mark "Initialisation"
+ (LCCaseSmallViewController *) controllerForCase:(LCCase *)initCase
{
	return [[[LCCaseSmallViewController alloc] initForCase:initCase] autorelease];
}

- (LCCaseSmallViewController *) initForCase:(LCCase *)initCase
{
	[self init];
	
	self.cas = initCase;
	[NSBundle loadNibNamed:@"CaseSmallView" owner:self];
	
	return self;
}

- (LCCaseSmallViewController *) init
{
	[super init];
	
	return self;
}

- (void) dealloc
{
	[view release];
	[objectController release];
	[super dealloc];
}

- (void) removeViewAndContent
{
	[view removeFromSuperviewWithoutNeedingDisplay];
	[objectController setContent:nil];
}

#pragma mark "Accessor Methods"
@synthesize cas;
@synthesize view;

@end
