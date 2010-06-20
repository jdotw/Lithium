//
//  LCErrorLog.m
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCErrorLog.h"

static LCErrorLog *masterLog;

@implementation LCErrorLog

#pragma mark "Constructors"

+ (LCErrorLog *) masterInit
{
	masterLog = [[LCErrorLog alloc] init];
	return masterLog;
}

- (LCErrorLog *) init
{
	[super init];
	
	errors = [[NSMutableArray array] retain];
	
	return self;
}

- (void) dealloc
{
	[errors release];
	[super dealloc];
}

#pragma mark "Accessors"

- (NSMutableArray *) errors
{ return errors; }

- (void) insertObject:(LCError *)error inErrorsAtIndex:(unsigned int)index
{
	[errors insertObject:error atIndex:index];
	if ([errors count] > 200)
	{ [errors removeObjectAtIndex:200]; }
}

- (void) removeObjectFromErrorsAtIndex:(unsigned int)index
{
	[errors removeObjectAtIndex:index];
}

+ (NSMutableArray *) masterErrors
{ return [masterLog errors]; }

+ (LCErrorLog *) masterLog
{ return masterLog; }

- (void) clearLog
{
	[self willChangeValueForKey:@"errors"];
	[errors removeAllObjects];
	[self didChangeValueForKey:@"errors"];
}

@synthesize errors;
@end
