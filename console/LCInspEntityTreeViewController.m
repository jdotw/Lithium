//
//  LCInspEntityTreeViewController.m
//  Lithium Console
//
//  Created by James Wilson on 2/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspEntityTreeViewController.h"


@implementation LCInspEntityTreeViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspEntityTreeViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];
	
	/* Expand all */
	int i;
	for (i=0; i < [outlineView numberOfRows]; i++)
	{ 
		if ([outlineView levelForRow:i] < 1)
		{ [outlineView expandItem:[outlineView itemAtRow:i]]; }
	}
	
	return self;
}

- (NSString *) nibName
{ return @"InspectorEntityTreeView"; }

- (float) defaultHeight
{ return 120.0; }

@end
