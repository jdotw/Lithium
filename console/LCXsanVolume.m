//
//  LCXsanVolume.m
//  Lithium Console
//
//  Created by James Wilson on 11/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXsanVolume.h"


@implementation LCXsanVolume

#pragma mark "Constructors"

+ (LCXsanVolume *) volumeWithObject:(LCEntity *)initEntity
{
	return [[[LCXsanVolume alloc] initWithObject:initEntity] autorelease];
}

- (LCXsanVolume *) initWithObject:(LCEntity *)initEntity
{
	[super init];

	/* Create array/dict */
	objects = [[NSMutableArray array] retain];
	objectDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Insert initial entity */
	[self insertObject:initEntity inObjectsAtIndex:0];
	
	return self;
}

- (void) dealloc
{ 
	[objects release];
	[objectDictionary release];
	[super dealloc];
}

#pragma mark "Object Accessors"

- (NSMutableArray *) objects
{ return objects; }

- (void) insertObject:(LCEntity *)object inObjectsAtIndex:(unsigned int)index
{
	[self willChangeValueForKey:@"children"];
	[objects insertObject:object atIndex:[objects count]];
	[objectDictionary setObject:object forKey:[[object entityAddress] addressString]];
	[self didChangeValueForKey:@"children"];
}

- (void) removeObjectFromObjectsAtIndex:(unsigned int)index
{
	[self willChangeValueForKey:@"children"];
	[objectDictionary removeObjectForKey:[[[objects objectAtIndex:index] entityAddress] addressString]];
	[objects removeObjectAtIndex:index];
	[self didChangeValueForKey:@"children"];
}

- (NSMutableDictionary *) objectDictionary
{ return objectDictionary; }

- (LCEntity *) bestObject
{
	/* FIX Should return the controller */
	return [objects objectAtIndex:0];
}

#pragma mark "Display Methods"

- (NSArray *) isBrowserTreeLeaf
{ return NO; }

- (NSArray *) children
{ return objects; }

- (NSString *) displayString
{
	if ([objects count] > 0) return [(LCEntity *) [objects objectAtIndex:0] desc];
	else return @"--Unknown--";
}

- (NSString *) uniqueIdentifier
{
	if ([objects count] > 0) return [(LCEntity *) [objects objectAtIndex:0] uniqueIdentifier];
	else return nil;
}

- (NSString *) opstateInteger
{
	if ([[self children] count] > 0)
	{
		return [[[self children] objectAtIndex:0] opstateInteger];
	}
	else
	{ return @"-1"; }
}

- (int) opState
{ return 0; }

- (BOOL) refreshInProgress
{ return NO; }

- (BOOL) selectable
{ return YES; }

@synthesize objects;
@synthesize objectDictionary;

- (NSString *) sortString
{ return self.displayString; }

@end
