//
//  LCLun.m
//  Lithium Console
//
//  Created by James Wilson on 12/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCLun.h"


@implementation LCLun

#pragma mark "Constructors"

+ (LCLun *) lunWithObject:(LCEntity *)initObject wwn:(NSString *)initWWN
{
	return [[[LCLun alloc] initWithObject:initObject wwn:initWWN] autorelease];
}

- (LCLun *) initWithObject:(LCEntity *)initObject wwn:(NSString *)initWWN
{
	[super init];
	
	uses = [[NSMutableArray array] retain];
	useDictionary = [[NSMutableDictionary dictionary] retain];
	[self setObject:initObject];
	[self setWwn:initWWN];
	
	return self;
}

- (void) dealloc
{
	[uses release];
	[useDictionary release];
	[object release];
	[wwn release];
	[super dealloc];
}

#pragma mark "Accessors"

- (LCEntity *) object
{ return object; }

- (void) setObject:(LCEntity *)newObject
{
	[object release];
	object = [newObject retain];
}

- (NSString *) wwn
{ return wwn ; }

- (void) setWwn:(NSString *)newWwn
{
	[wwn release];
	wwn = [[NSString stringWithString:newWwn] retain];
}

- (NSMutableArray *) uses
{ return uses; }

- (void) insertObject:(id)obj inUsesAtIndex:(unsigned int)index
{ 
	[uses insertObject:obj atIndex:index]; 
}

- (void) removeObjectFromUsesAtIndex:(unsigned int)index
{ [uses removeObjectAtIndex:index]; }

- (NSMutableDictionary *) useDictionary
{ return useDictionary; }

@synthesize uses;
@synthesize useDictionary;
@end
