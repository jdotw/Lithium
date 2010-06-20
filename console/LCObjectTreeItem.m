//
//  LCObjectTreeItem.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCObjectTreeItem.h"


@implementation LCObjectTreeItem

#pragma mark "Constructor"

- (id) initWithObject:(LCObject *)initObject;
{
	self = [super init];
	
	self.object = initObject;
	
	return self;
}

- (void) dealloc
{
	[object release];
	[children release];
	[super dealloc];
}

#pragma mark Accessors

@synthesize object;
- (LCEntity *) entity
{ return (LCEntity *) object; }

@synthesize rowHeight;

@synthesize children;
- (void) setChildren:(NSMutableArray *)value
{
	[children release];
	children = [value mutableCopy];
}

@synthesize isObjectTreeLeaf;
- (BOOL) isGroupTreeLeaf
{ return YES; }

- (NSString *) uniqueIdentifier
{ return [[object entityAddress] addressString]; }

@end
