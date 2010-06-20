//
//  LCInspXRaidListItem.m
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidListItem.h"
#import "LCLun.h"

#import "LCInspXRaidListViewController.h"
#import "LCEntity.h"
#import "LCMetric.h"
#import "LCCustomer.h"

@implementation LCInspXRaidListItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspXRaidListItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"Xserve RAIDs"];
	
	if ([target object])
	{
		/* Target is a visible disk object */
		[self createViewControllersForObjects:[NSArray arrayWithObject:[target object]] metaDataObjects:nil];
	}
	else if ([[target name] hasPrefix:@"xsanvolsp"])
	{
		/* Got a container that holds a list of storage pools */
		NSEnumerator *poolEnum = [[target children] objectEnumerator];
		LCEntity *poolObj;
		NSMutableArray *aggregateObjs = [NSMutableArray array];
		NSMutableArray *metadataObjs = [NSMutableArray array];
		while (poolObj = [poolEnum nextObject])
		{
			/* Loop through each pool */
			NSString *spLunContName = [NSString stringWithFormat:@"xsansplun_%@", [poolObj name]];
			LCEntity *spLunCont = [[target device] childNamed:spLunContName];
			LCMetric *metadataMet = (LCMetric *) [poolObj childNamed:@"metadata"];
			[aggregateObjs addObjectsFromArray:[spLunCont children]];
			if ([[[metadataMet currentValue] rawValueString] intValue] == 1)
			{
				/* Metadata pool */
				[metadataObjs addObjectsFromArray:[spLunCont children]];
			}
		}
		[self createViewControllersForObjects:aggregateObjs metaDataObjects:metadataObjs];
	}
	else
	{
		/* Target is a container of visdisk objects */
		[self createViewControllersForObjects:[target children] metaDataObjects:nil];
	}
	
	return self;
}

- (void) createViewControllersForObjects:(NSArray *)objArray metaDataObjects:(NSArray *)metadataObjArray
{
	NSMutableArray *devMetadataArrays = [NSMutableArray array];
	NSMutableDictionary *raidDictionary = [NSMutableDictionary dictionary];
	LCEntity *obj;
	for (obj in objArray)
	{
		/* Check WWN */
		if (![[(LCMetric *) [[obj childrenDictionary] objectForKey:@"wwn"] currentValue] rawValueString])
		{ continue; }
		
		/* Get LCLun for Visible Disk */
		NSMutableString *visWWN = [NSMutableString stringWithString:[[(LCMetric *) [[obj childrenDictionary] objectForKey:@"wwn"] currentValue] rawValueString]];
		[visWWN replaceCharactersInRange:NSMakeRange(0,1) withString:@"6"];
		LCLun *lun = [[[(LCCustomer *)[obj customer] lunList] wwnDictionary] objectForKey:visWWN];
		if (lun)
		{ 
			NSMutableArray *devLunArray = [raidDictionary objectForKey:[[[[lun object] device] entityAddress] addressString]];
			if (devLunArray)
			{
				/* Existing device/lun array */
				[devLunArray addObject:[lun object]];
			}
			else
			{
				/* New dev/lun array */
				if ([lun object])
				{
					devLunArray = [NSMutableArray arrayWithObject:[lun object]];
					[raidDictionary setObject:devLunArray forKey:[[[[lun object] device] entityAddress] addressString]];
				}
			}
			if ([metadataObjArray containsObject:obj])
			{ [devMetadataArrays addObject:[lun object]]; }
		}
	}
	
	/* We now have a dictionary of arrays where each dictionary
		* is an array of objects that are arrays within that device
		*/
	NSEnumerator *devArraysEnum = [raidDictionary objectEnumerator];
	NSArray *devArrays;
	while (devArrays = [devArraysEnum nextObject])
	{
		LCInspXRaidListViewController *viewController;
		viewController = [[LCInspXRaidListViewController alloc] initWithRaid1Arrays:devArrays raid2Arrays:[devArraysEnum nextObject] metadataArrays:devMetadataArrays];
		if ([devArrays count] > 1) [viewController setNumUnits:2];
		[viewController updateImages];
		[self insertObject:viewController inViewControllersAtIndex:[viewControllers count]];
		[viewController autorelease];
	}
}

- (BOOL) allowsResize
{ return NO; }

@end
