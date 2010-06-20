//
//  LCPerformanceItem.m
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCPerformanceItem.h"


@implementation LCPerformanceItem

#pragma mark "Constructors"

- (LCPerformanceItem *) init
{
	[super init];
	items = [[NSMutableArray array] retain];
	itemDictionary = [[NSMutableDictionary dictionary] retain];
	return self;
}

- (void) dealloc
{
	[items release];
	[itemDictionary release];
	[processID release];
	[cpuPercent release];
	[vSize release];
	[rSize release];
	[super dealloc];
}

#pragma mark "Items"

- (NSMutableArray *) items;
{ return items; }

- (NSMutableDictionary *) itemDictionary
{ return itemDictionary; }

- (void) insertObject:(id)object inItemsAtIndex:(unsigned int)index
{ 
	[items insertObject:object atIndex:index];
	[itemDictionary setObject:object forKey:[object processID]];
}

- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{ 
	[itemDictionary removeObjectForKey:[[items objectAtIndex:index] processID]];
	[items removeObjectAtIndex:index]; 
}

- (BOOL) isLeafNode
{ return NO; }

- (BOOL) isGroup
{ return NO; }

#pragma mark "Stats Accessors"

- (NSString *) processID
{ return processID; }
- (void) setProcessID:(NSString *)value
{
	[processID release];
	processID = [value retain];
}

- (NSString *) cpuPercent
{ return cpuPercent; }
- (void) setCpuPercent:(NSString *)value
{
	[cpuPercent release];
	cpuPercent = [value retain];
}

- (NSString *) vSize
{ return vSize; }
- (void) setVSize:(NSString *)value
{
	[vSize release];
	vSize = [value retain];
}

- (NSString *) rSize
{ return rSize; }
- (void) setRSize:(NSString *)value
{
	[rSize release];
	rSize = [value retain];
}

#pragma mark "Display Strings"

- (NSString *) displayString
{ return [NSString stringWithFormat:@"Process %@", processID]; }

@end
