//
//  LCInspRecordedMetricsViewController.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspRecordedMetricsViewController.h"
#import "LCMetricGraphDocument.h"

@implementation LCInspRecordedMetricsViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspRecordedMetricsViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	self.filterPredicate = [NSPredicate predicateWithFormat:@"recordMethod == 1 AND recordEnabled == 1"];

	[super initWithTarget:initTarget];
	
	[tableView setRoundedSelection:YES];

	return self;
}

- (void) dealloc
{
	[filterPredicate release];
	[super dealloc];
}

- (float) defaultHeight
{ 
	return 120.0; 
}

- (IBAction) tableViewDoubleClicked:(NSArray *)selectedObjects
{
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = selectedObjects;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];		
}

@synthesize filterPredicate;

- (NSString *) nibName
{ return @"InspectorRecordedMetricsView"; }


@end
