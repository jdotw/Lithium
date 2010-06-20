//
//  LCInspXRaidLUNItem.m
//  Lithium Console
//
//  Created by James Wilson on 18/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidLUNItem.h"

#import "LCEntity.h"
#import "LCCustomer.h"
#import "LCLun.h"
#import "LCInspXRaidLUNViewController.h"

@implementation LCInspXRaidLUNItem

+ (id) itemWithTarget:(id)initTarget forController:(id)initController
{
	return [[[LCInspXRaidLUNItem alloc] initWithTarget:initTarget forController:initController] autorelease];
}

- (id) initWithTarget:(id)initTarget forController:(id)initController
{
	[super initWithTarget:initTarget forController:initController];
	
	[self setDisplayString:@"RAID Array Usage"];

	/* Get Array Objects */
	NSMutableArray *arrayObjs = nil;
	if ([target device] == target)
	{
		/* Target it an Xserve RAID Device */
		arrayObjs = [NSMutableArray array];
		for (LCEntity *cont in [target children])
		{
			if ([[cont name] hasPrefix:@"xrarray"] && [[cont children] count] > 0) 
			{
				[arrayObjs addObject:[[cont children] objectAtIndex:0]]; 
			}
		}
	}
	else 
	{
		/* Target is either an array container or array object (always 1 in the same) */
		if ([target container] == target)
		{ arrayObjs = [[target children] mutableCopy]; }
		else if ([target object] == target)
		{ arrayObjs = [NSMutableArray arrayWithObject:target]; }
	}
	
	/* Create view controllers */
	for (LCEntity *array in arrayObjs)
	{
		/* Get the LCLun */
		LCLun *lun = [[[(LCCustomer *)[array customer] lunList] lunDictionary] objectForKey:[[array entityAddress] addressString]];
		
		/* Create View Controller */
		LCInspXRaidLUNViewController *viewController;
		viewController = (LCInspXRaidLUNViewController *) [[LCInspXRaidLUNViewController alloc] initWithTarget:array lun:lun];
		[self insertObject:viewController inViewControllersAtIndex:[viewControllers count]];
		[viewController autorelease];
	}
	
	return self;
}

@end
