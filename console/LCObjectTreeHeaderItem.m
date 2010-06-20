//
//  LCObjectTreeHeaderItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeHeaderItem.h"

#import "LCObjectTreeMetricItem.h"

@implementation LCObjectTreeHeaderItem

- (id) initWithObject:(LCObject *)initObject showAllMetrics:(BOOL)showAll
{
	self = [super initWithObject:initObject];

	if (self)
	{
		self.rowHeight = 16.0;
		self.isObjectTreeLeaf = NO;
		children = [[NSMutableArray array] retain];
		LCObjectTreeMetricItem *metricItem = [[LCObjectTreeMetricItem alloc] initWithObject:object showAllMetrics:showAll];
		[children addObject:metricItem];
		[metricItem autorelease];		
	}
	
	return self;
}


@end
