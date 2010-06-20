//
//  LCPerformanceGroup.m
//  LCAdminTools
//
//  Created by James Wilson on 3/08/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCPerformanceGroup.h"
#import "LCPerformanceProcess.h"

@implementation LCPerformanceGroup

#pragma mark "Constructors"

+ (LCPerformanceGroup *) performanceGroupNamed:(NSString *)initName
{ 
	return [[[LCPerformanceGroup alloc] initWithName:initName] autorelease];
}

- (LCPerformanceGroup *) initWithName:(NSString *)initName
{
	[super init];
	[self setName:initName];
	return self;
}

- (void) dealloc
{ 
	[name release];
	[super dealloc];
}

#pragma mark "Total Calculations"

- (void) updateTotals
{
	float cpuTotal = 0.0;
	unsigned long rSizeTotal = 0;
	
	NSEnumerator *itemEnum = [[self items] objectEnumerator];
	LCPerformanceProcess *process;
	while (process=[itemEnum nextObject])
	{
		cpuTotal += [[process cpuPercent] floatValue];
		rSizeTotal += (unsigned long) [[process rSize] intValue];
	}
	
	[self setCpuPercent:[NSString stringWithFormat:@"%.1f", cpuTotal]];
	[self setRSize:[NSString stringWithFormat:@"%lu", rSizeTotal]];
}

#pragma mark "Obsolescence Checking"

- (void) checkProcessExistence:(NSMutableDictionary *)processDict
{
	NSMutableArray *removeList = [NSMutableArray array];
	for (LCPerformanceItem *item in items)
	{
		if (![processDict objectForKey:[item processID]])
		{ 
			/* Process is extinct */
			[removeList addObject:item];
		}
	}
	for (LCPerformanceItem *item in removeList)
	{ [self removeObjectFromItemsAtIndex:[items indexOfObject:item]]; }
}

#pragma mark "Accessors"

- (NSString *) name
{ return name; }

- (void) setName:(NSString *)string
{
	[name release];
	name = [string retain];
}

- (NSString *) displayString
{ return name; }

- (BOOL) isGroup
{ return YES; }

@end
