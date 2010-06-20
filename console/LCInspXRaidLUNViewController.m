//
//  LCInspXRaidLUNViewController.m
//  Lithium Console
//
//  Created by James Wilson on 18/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidLUNViewController.h"


@implementation LCInspXRaidLUNViewController

+ (LCInspXRaidLUNViewController *) itemWithTarget:(id)initTarget lun:(LCLun *)initLun
{
	return [[[LCInspXRaidLUNViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget lun:(LCLun *)lun
{
	[super initWithTarget:initTarget];
	
	[raidView setRaid1Arrays:[NSArray arrayWithObject:target]];
	[raidView setNumUnits:2];
	[raidView setLun:lun];
	[raidView updateImages];
	[self setRowHeight:70.0];
	
	return self;
}

- (float) defaultHeight
{ return 70.0; }

- (void) setController:(id)newController
{
	[super setController:newController];
	[raidView setController:controller];
}

- (NSString *) nibName
{
	return @"InspectorXRaidLUNView";
}


@end
