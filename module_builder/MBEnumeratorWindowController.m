//
//  MBEnumeratorWindowController.m
//  ModuleBuilder
//
//  Created by James Wilson on 13/05/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "MBEnumeratorWindowController.h"
#import "MBMetric.h"
#import "MBEnum.h"

@implementation MBEnumeratorWindowController

@synthesize metric;

- (IBAction) addClicked:(id)sender
{
	[metric insertObject:[MBEnum enumWithLabel:nil forValue:nil] inEnumeratorsAtIndex:self.metric.enumerators.count];
}

- (IBAction) removeClicked:(id)sender
{
	for (MBEnum *enumerator in [enumeratorArrayController selectedObjects])
	{
		[metric removeObjectFromEnumeratorsAtIndex:[metric.enumerators indexOfObject:enumerator]];	
	}
}

- (void) windowWillClose:(NSNotification *)notification
{
	[objectController setContent:nil];
}

- (void) dealloc
{
	[metric release];
	[super dealloc];
}

@end
