//
//  LCInspCaseHistoryViewController.m
//  Lithium Console
//
//  Created by James Wilson on 10/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCInspCaseHistoryViewController.h"
#import "LCCaseController.h"

@implementation LCInspCaseHistoryViewController

+ (LCInspCaseHistoryViewController *) itemWithTarget:(id)initTarget caseList:(LCCaseList *)initCaseList
{
	return [[[LCInspCaseHistoryViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget caseList:(LCCaseList *)initCaseList
{
	[super initWithTarget:initTarget];
	
	self.caseList = initCaseList;
	
	[tableView setRoundedSelection:YES];
	
	return self;
}

- (float) defaultHeight
{ 
	return 80.0;
}

- (void) tableViewDoubleClicked:(NSArray *)selectedObjects
{
	for (LCCase *cas in selectedObjects)
	{
		[[LCCaseController alloc] initWithCase:cas];
	}
}

@synthesize caseList;

- (NSString *) nibName
{ return @"InspectorCaseHistoryView"; }

@end
