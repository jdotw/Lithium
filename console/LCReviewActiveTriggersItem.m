//
//  LCReviewActiveTriggersItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCReviewActiveTriggersItem.h"


@implementation LCReviewActiveTriggersItem

#pragma mark "Constructors"

+ (LCReviewActiveTriggersItem *) itemForEntity:(LCEntity *)initEntity
{
	return [[[LCReviewActiveTriggersItem alloc] initForEntity:initEntity] autorelease];
}

- (LCReviewActiveTriggersItem *) initForEntity:(LCEntity *)initEntity
{
	[super init];

	self.entity = initEntity;
	
	return self;
}

#pragma mark "Accessors"

@synthesize selected;
@synthesize entity;

@end
