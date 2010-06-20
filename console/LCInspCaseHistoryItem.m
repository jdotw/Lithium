//
//  LCInspCaseHistoryItem.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspCaseHistoryItem.h"

#import "LCInspCaseHistoryViewController.h"

@implementation LCInspCaseHistoryItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspCaseHistoryItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Case History (...)"];
	
	self.caseList = [[LCCaseList new] autorelease];
	caseList.customer = [(LCEntity *)target customer];
	caseList.delegate = self;
	caseList.entity = target;
	[caseList highPriorityRefresh];	
	
	LCInspCaseHistoryViewController *viewController = [[LCInspCaseHistoryViewController alloc] initWithTarget:target caseList:caseList];
	[self insertObject:viewController inViewControllersAtIndex:0];
	[viewController autorelease];
	
	return self;
}

- (void) dealloc
{
	[caseList setDelegate:nil];
	[caseList release];
	[super dealloc];
}

- (void) caselistRefreshFinished:(id)sender
{
	/* Set title */
	if ([[caseList cases] count] >= 20)
	{ [self setDisplayString:@"Case History (20+)"]; }
	else
	{ [self setDisplayString:[NSString stringWithFormat:@"Case History (%i)", [[caseList cases] count]]]; }
}

@synthesize caseList;

@end
