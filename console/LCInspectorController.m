//
//  LCInspectorController.m
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorController.h"
#import "LCEntity.h"
#import "LCInspEntityTreeItem.h"
#import "LCInspRecordedMetricsItem.h"
#import "LCInspMiniGraphItem.h"
#import "LCInspPropertiesItem.h"
#import "LCInspTrendItem.h"
#import "LCInspDeviceRefreshItem.h"
#import "LCInspServiceConfigItem.h"
#import "LCInspActiveIncidentsItem.h"
#import "LCInspPastIncidentsItem.h"
#import "LCInspXRaidListItem.h"
#import "LCInspXRaidLUNItem.h"
#import "LCInspIncidentHistoryItem.h"
#import "LCInspCaseHistoryItem.h"
#import "LCInspRelatedIncidentsItem.h"
#import "LCCase.h"
#import "LCMetric.h"
#import "LCInspMetricValuesItem.h"

/*
 * LCInspectorController
 * |
 * |--->Items
 *       |
 *       |-> LCInspectorItem == "Header View -- Fixed Width"
 *              |
 *              |-> LCInspectorViewController == "Data View -- Variable width"
 *              |-> (LCInspectorViewController) Multiple Optional
 */

@implementation LCInspectorController

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	items = [[NSMutableArray array] retain];
}

- (void) dealloc
{
	for (LCInspectorItem *item in items)
	{ [item removedFromInspector]; }
	[items release];
	[target release];
	
	[super dealloc];
}

#pragma mark "Target Methods"

@synthesize target;
- (void) setTarget:(id)newTarget
{
	/* Set Target */
	[target release];
	target = [newTarget retain];
	
	/* Reset items */
	[self resetItems];
}

#pragma mark "Item Management"

- (void) resetItems
{
	/* Clear out the old */
	while ([items count] > 0)
	{ 
		LCInspectorItem *item = [items objectAtIndex:0];
		[item removedFromInspector];
		[self removeObjectFromItemsAtIndex:0]; 
	}
	
	/*
	 * Entity Items 
	 */

	if ([[target class] isSubclassOfClass:[LCEntity class]])
	{
		LCEntity *entity = target;
		
		/* Create entity-default items */
		[self insertObject:[LCInspEntityTreeItem itemWithTarget:target forController:self]
			inItemsAtIndex:[items count]];

		/* Device-Specific */
		if ([entity device] == entity)
		{
			/* Refresh Info */
			[self insertObject:[LCInspDeviceRefreshItem itemWithTarget:target  forController:self]
				inItemsAtIndex:[items count]];
			
			/* Active Incidents */
			[self insertObject:[LCInspActiveIncidentsItem itemWithTarget:target  forController:self]
				inItemsAtIndex:[items count]];
			
			/* Xserve RAID Device */
			if ([[[entity device] vendor] isEqualToString:@"xraid"])
			{
				[self insertObject:[LCInspXRaidLUNItem itemWithTarget:target  forController:self]
					inItemsAtIndex:[items count]];
			}
		}		

		/* Container-Specific */
		if ([entity container] == entity)
		{
			/* Mini Graph */
//			[self insertObject:[LCInspMiniGraphItem itemWithTarget:target forController:self]
//				inItemsAtIndex:[items count]];			

			/* Xsan Visible Disk */
			if ([[[entity container] name] isEqualToString:@"xsanvisdisk"] || 
				[[[entity container] name] hasPrefix:@"xsansplun"] ||
				[[[entity container] name] hasPrefix:@"xsanvolsp"])
			{
				[self insertObject:[LCInspXRaidListItem itemWithTarget:target forController:self]
					inItemsAtIndex:[items count]];
			}			

			/* Xserve RAID Array Container */
			if ([[[entity container] name] hasPrefix:@"xrarray"])
			{
				[self insertObject:[LCInspXRaidLUNItem itemWithTarget:target  forController:self]
					inItemsAtIndex:[items count]];
			}			
		}

		/* Object-Specific */
		if ([entity object] == entity)
		{
			/* Mini Graph */
			[self insertObject:[LCInspMiniGraphItem itemWithTarget:target forController:self]
				inItemsAtIndex:[items count]];

			/* Other-Metrics Mini Graph */
			[self insertObject:[LCInspMiniGraphItem itemForMinorMetricsWithTarget:target forController:self]
				inItemsAtIndex:[items count]];			

			/* Recorded Metrics */
			[self insertObject:[LCInspRecordedMetricsItem itemWithTarget:target forController:self]
				inItemsAtIndex:[items count]];
			
			/* Trend Analysis */
			if ([LCInspTrendItem targetHasTriggers:target])
			{
				[self insertObject:[LCInspTrendItem itemWithTarget:target forController:self]
					inItemsAtIndex:[items count]];
			}
			
			/* Service */
			if ([[[entity container] name] isEqualToString:@"service"])
			{
				[self insertObject:[LCInspServiceConfigItem itemWithTarget:target forController:self]
					inItemsAtIndex:[items count]];
			}

			/* Xsan Visible Disk */
			if ([[[entity container] name] isEqualToString:@"xsanvisdisk"] ||
				[[[entity container] name] hasPrefix:@"xsansplun_"])
			{
				[self insertObject:[LCInspXRaidListItem itemWithTarget:target forController:self]
					inItemsAtIndex:[items count]];
			}

			/* Xsan Volume Object */
			if ([[[entity container] name] isEqualToString:@"xsanvol"])
			{
				NSString *lunContName = [NSString stringWithFormat:@"xsanvolsp_%@", [entity name]];
				LCEntity *lunContainer = [[[entity device] childrenDictionary] objectForKey:lunContName];
				if (lunContainer)
				{
					[self insertObject:[LCInspXRaidListItem itemWithTarget:lunContainer forController:self]
						inItemsAtIndex:[items count]];
				}
			}			
			
			/* Xsan Volume Storage Pool Object */
			if ([[[entity container] name] hasPrefix:@"xsanvolsp_"])
			{
				NSString *lunContName = [NSString stringWithFormat:@"xsansplun_%@", [entity name]];
				LCEntity *lunContainer = [[[entity device] childrenDictionary] objectForKey:lunContName];
				if (lunContainer)
				{
					[self insertObject:[LCInspXRaidListItem itemWithTarget:lunContainer forController:self]
						inItemsAtIndex:[items count]];
				}
			}						

			/* Xserve RAID Array Container */
			if ([[[entity container] name] hasPrefix:@"xrarray"])
			{
				[self insertObject:[LCInspXRaidLUNItem itemWithTarget:target  forController:self]
					inItemsAtIndex:[items count]];
			}			
		}

		/* Past Incidents */
		[self insertObject:[LCInspPastIncidentsItem itemWithTarget:target forController:self]
			inItemsAtIndex:[items count]];
	}

	/*
	 * Incident Items
	 */	
	
	if ([[target class] isSubclassOfClass:[LCIncident class]])
	{
		LCIncident *inc = (LCIncident *) target;

		/* Mini Graph */
		if ([(LCMetric *)[[inc entity] metric] recordEnabled])
		{
			[self insertObject:[LCInspMiniGraphItem itemWithTarget:[[inc entity] object] forController:self]
				inItemsAtIndex:[items count]];
		}
		
		/* Past Cases */
		[self insertObject:[LCInspCaseHistoryItem itemWithTarget:[inc entity] forController:self]
			inItemsAtIndex:[items count]];

		/* Related Incidents */
		[self insertObject:[LCInspRelatedIncidentsItem itemWithTarget:[inc entity] forController:self]
			inItemsAtIndex:[items count]];		
		
		/* Past Incidents */
		[self insertObject:[LCInspIncidentHistoryItem itemWithTarget:[inc entity] forController:self]
			inItemsAtIndex:[items count]];
		
		
	}

	/*
	 * Case Items
	 */	
	
	if ([[target class] isSubclassOfClass:[LCCase class]])
	{
		LCCase *cas = (LCCase *) target;
		
		/* Mini Graphs */
		NSMutableArray *metrics = [NSMutableArray array];
		for (LCEntity *entity in [[cas entityList] entities])
		{ 
			if ([entity metric]) [metrics addObject:[entity metric]]; 
			else if ([entity trigger]) [metrics addObject:[entity parent]];
		}
		
		[self insertObject:[LCInspMiniGraphItem itemWithTarget:cas metrics:metrics forController:self]
			inItemsAtIndex:[items count]];		
		
		/* Metric Values */
		[self insertObject:[LCInspMetricValuesItem itemWithTarget:cas metrics:metrics forController:self]
			inItemsAtIndex:[items count]];
	}	
	
	/*
	 * Common
	 */
	
	/* Expand all */
	int i;
	float height = 18 + 32;
	for (i=0; i < [outlineView numberOfRows]; i++)
	{ 
		if ([outlineView levelForRow:i] == 0 && [[[outlineView itemAtRow:i] representedObject] expandByDefault])
		{ [outlineView expandItem:[outlineView itemAtRow:i]]; }
		height = height + [outlineView outlineView:outlineView heightOfRowByItem:[outlineView itemAtRow:i]];
	}
}

@synthesize items;

- (void) insertObject:(id)obj inItemsAtIndex:(unsigned int)index
{
	[obj setController:self];
	[items insertObject:obj atIndex:index];
}

- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[items removeObjectAtIndex:index];
}

#pragma mark "Resize Arbitration"

- (void) adjustItemAbove:(LCInspectorItem *)item viewHeightsBy:(float)delta
{
	/* Get Index  of item */
	int index = [items indexOfObject:item];
	if (index < 1 || index == NSNotFound) return;
	
	/* Get item "above" */
	LCInspectorItem *itemAbove = [items objectAtIndex:index-1];
	[itemAbove adjustViewHeightsBy:delta];
	[outlineView resetSubviewsOnly];
}

- (void) adjustItemAboveToDefaultHeight:(LCInspectorItem *)item
{
	/* Get Index  of item */
	int index = [items indexOfObject:item];
	if (index < 1 || index == NSNotFound) return;
	
	/* Get item "above" */
	LCInspectorItem *itemAbove = [items objectAtIndex:index-1];
	[itemAbove adjustViewHeightsToDefault];
	[outlineView resetSubviewsOnly];	
}

- (void) adjustAllItemsToDefaultHeight
{
	/* Get Index  of item */
	LCInspectorItem *item;
	for (item in items)
	{
		[item adjustViewHeightsToDefault];
		[outlineView resetSubviewsOnly];	
	}
}

- (void) resizeDragFinished
{
	[[NSUserDefaults standardUserDefaults] synchronize];
	[outlineView reloadData];
}

- (BOOL) itemAboveAllowsResize:(LCInspectorItem *)item 
{ 
	int index = [items indexOfObject:item];
	if (index < 1 || index == NSNotFound) return NO;
	LCInspectorItem *itemAbove = [items objectAtIndex:index-1];
	return [itemAbove allowsResize];
}

@end
