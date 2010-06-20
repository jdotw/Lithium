//
//  LCGroupTreeOutlineItem.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCGroupTreeOutlineItem.h"

@implementation LCGroupTreeOutlineItem

#pragma mark "Constructor"

- (id) initWithEntity:(LCEntity *)initEntity;
{
	self = [super init];
	
	self.entity = initEntity;
	
	return self;
}

- (void) dealloc
{
	[entity release];
	[children release];
	[super dealloc];
}

#pragma mark Accessors

@synthesize entity;
@synthesize rowHeight;

@synthesize children;
- (void) setChildren:(NSMutableArray *)value
{
	[children release];
	children = [value mutableCopy];
}

@synthesize isGroupTreeLeaf;

- (NSString *) uniqueIdentifier
{ return [[entity entityAddress] addressString]; }

@end
