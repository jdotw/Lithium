//
//  LCInspXRaidListViewController.m
//  Lithium Console
//
//  Created by James Wilson on 17/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspXRaidListViewController.h"


@implementation LCInspXRaidListViewController

- (LCInspXRaidListViewController *) initWithRaid1Arrays:(NSArray *)raid1Arrays raid2Arrays:(NSArray *)raid2Arrays metadataArrays:(NSArray *)metadataArrays
{
	[super initWithTarget:[raid1Arrays objectAtIndex:0]];
	
	[raidView setMetadataArrays:metadataArrays];
	[raidView setRaid1Arrays:raid1Arrays];
	if (raid2Arrays)
	{
		[raidView setRaid2Arrays:raid2Arrays];
		[raidView setNumUnits:2];
		[self setRowHeight:70.0];
	}
	else
	{ 
		[raidView setNumUnits:1]; 
		[self setRowHeight:110.0];
	}
	
	return self;
}

- (void) setNumUnits:(int)num
{ [raidView setNumUnits:2]; }

- (void) updateImages
{ [raidView updateImages]; }

- (void) setController:(id)newController
{
	[super setController:newController];
	[raidView setController:controller];
}

- (NSString *) nibName
{ return @"InspectorXRaidView"; }

@end
