//
//  LCGroupTreeOutlineEntityItem.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCGroupTreeOutlineEntityItem.h"

#import "LCObjectTreeMetricItem.h"

@implementation LCGroupTreeOutlineEntityItem

- (id) initWithEntity:(LCEntity *)initEntity
{
	self = [super initWithEntity:initEntity];
	if (!self) return nil;
	
	self.rowHeight = 16.0;
	
	if (entity.type == 5)
	{
		/* Metric */
		children = [[NSMutableArray array] retain];
		LCObjectTreeMetricItem *metricItem = [[LCObjectTreeMetricItem alloc] initWithObject:(LCObject *)entity showAllMetrics:NO];
		[children addObject:metricItem];
		[metricItem autorelease];		
		self.isGroupTreeLeaf = NO;
	}		
	else
	{
		self.isGroupTreeLeaf = YES;
	}

	return self;
}


@end
