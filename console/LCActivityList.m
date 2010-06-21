//
//  LCActivityList.m
//  Lithium Console
//
//  Created by James Wilson on 30/10/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActivityList.h"

static LCActivityList *masterList = nil;

@implementation LCActivityList

#pragma mark "Initialisation"

+ (LCActivityList *) list
{ 
	return [[[LCActivityList alloc] init] autorelease];
}

- (LCActivityList *) init
{
	[super init];
	activities = [[NSMutableArray array] retain];
	return self;
}

+ (LCActivityList *) masterInit
{
	masterList = [[LCActivityList list] retain];
	return masterList;
}

#pragma mark "Accessor Methods"

+ (LCActivityList *) masterList
{ return masterList; }

- (NSMutableArray *) activities
{ return activities; }

- (void) insertObject:(LCActivity *)activity inActivitiesAtIndex:(unsigned int)index
{
	[activity setActivityID:[self allocateActivityID]];
	[activities insertObject:activity atIndex:index];
}

- (void) removeObjectFromActivitiesAtIndex:(unsigned int)index
{
	[activities removeObjectAtIndex:index];
}

- (unsigned long) allocateActivityID
{
	nextActivityID++;
	return nextActivityID;
}

@synthesize nextActivityID;
@synthesize activities;
@end
