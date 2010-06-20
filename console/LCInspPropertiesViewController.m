//
//  LCInspPropertiesViewController.m
//  Lithium Console
//
//  Created by James Wilson on 5/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspPropertiesViewController.h"


@implementation LCInspPropertiesViewController

+ (id) itemWithTarget:(id)initTarget
{
	return [[[LCInspPropertiesViewController alloc] initWithTarget:initTarget] autorelease];
}

- (id) initWithTarget:(id)initTarget
{
	[super initWithTarget:initTarget];

	/* Setup properties */
	properties = [[NSMutableArray array] retain];
	for (NSString *key in [[target properties] allKeys])
	{
		NSMutableDictionary *entry = [NSMutableDictionary dictionary];
		[entry setObject:key forKey:@"key"];
		[entry setObject:[[target properties] objectForKey:key] forKey:@"value"];
		[properties addObject:entry];
	}		
	
	return self;
}

- (float) defaultHeight
{ return 120.0; }

@synthesize properties;

- (NSString *) nibName
{ return @"InspectorPropertiesView"; }

@end
