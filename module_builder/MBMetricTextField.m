//
//  MBMetricTextField.m
//  ModuleBuilder
//
//  Created by James Wilson on 7/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MBMetricTextField.h"

#import "MBMetric.h"
#import "MBRateMetric.h"
#import "MBPercentMetric.h"
#import "ModuleDocument.h"
#import "MBContainer.h"

@implementation MBMetricTextField

#pragma mark "Awake From NIB"

- (void) awakeFromNib 
{
	[self registerForDraggedTypes:[NSArray arrayWithObject:@"MBEntity"]];
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	if ([self isEnabled])
	{
		[self setBackgroundColor:[NSColor colorWithDeviceRed:199.0/255.0 green:214.0/255.0 blue:242.0/255.0 alpha:1.0]];
		return NSDragOperationMove;
	}
	else
	{ return NSDragOperationNone; }
}

- (void)draggingExited:(id < NSDraggingInfo >)sender
{
	[self setBackgroundColor:[NSColor whiteColor]];
}

- (void)draggingEnded:(id < NSDraggingInfo >)sender
{
	[self setBackgroundColor:[NSColor whiteColor]];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
    NSPasteboard *pboard;
    pboard = [info draggingPasteboard];
	
	/* Get Entities */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"MBEntity"];
	for (NSMutableDictionary *properties in propertiesArray)
	{
		/* Find the metric */
		ModuleDocument *document = [(MBContainer *)[(MBMetric *)[viewController representedObject] parent] document];
		MBContainer *droppedContainer = nil;
		for (MBContainer *container in [document containers])
		{ 
			if ([[container desc] isEqualToString:[properties objectForKey:@"container"]])
			{
				droppedContainer = container;
				break; 
			}
		}
		if (!droppedContainer)
		{ 
			continue;
		}
		MBMetric *droppedMetric = nil;
		for (MBMetric *metric in [droppedContainer children])
		{
			if ([[metric desc] isEqualToString:[properties objectForKey:@"metric"]])
			{
				droppedMetric = metric;
				break;
			}
		}
		if (!droppedMetric)
		{ 
			continue;
		}

		/* Add to Host Metric */
		[[viewController representedObject] setValue:droppedMetric forKey:metricProperty];
	}	
	return YES;
}

@synthesize metricProperty;

@end
