//
//  LCBWRepGroup.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepGroup.h"


@implementation LCBWRepGroup

#pragma mark "Constructors"

+ (LCBWRepGroup *) group
{
	return [[[LCBWRepGroup alloc] init] autorelease];
}

- (LCBWRepGroup *) init
{
	[super init];
	deviceItems = [[NSMutableDictionary dictionary] retain];
	[self setArrangeByDevice:YES];
	[self setType:BWREP_ITEM_GROUP];
	return self;
}

- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];	
    return self;
}

#pragma mark "Children Methods"

- (NSArray *) displayChildren
{
	if ([self arrangeByDevice])
	{
		/* Return unchanged children list */
		return [self children];
	}
	else
	{
		/* Aggregate list of interfaces */
		NSMutableArray *array = [NSMutableArray array];
		NSEnumerator *childEnum = [[properties objectForKey:@"children"] objectEnumerator];
		LCBWRepItem *item;
		while (item = [childEnum nextObject])
		{
			switch ([item type])
			{
				case BWREP_ITEM_DEVICE:
					[array addObjectsFromArray:[item children]];
					break;
				default:
					[array addObject:item];
			}
		}
		return array;
	}
}

#pragma mark "Arrangement"

- (BOOL) arrangeByDevice
{
	return [[properties objectForKey:@"arrangeByDevice"] boolValue];
}

- (void) setArrangeByDevice:(BOOL)value
{
	[self willChangeValueForKey:@"displayChildren"];
	[properties setObject:[NSNumber numberWithBool:value] forKey:@"arrangeByDevice"];
	[self didChangeValueForKey:@"displayChildren"];
	[reportDocument updateChangeCount:NSChangeDone];
}

#pragma mark Description

- (BOOL) canEditDescription
{ return YES; }

- (NSString *) displayDescription
{ return [properties objectForKey:@"displayDescription"]; }

- (void) setDisplayDescription:(NSString *)value
{
	if ([self displayDescription] && reportDocument)
	{
		/* Update documents group dictionary */
		[reportDocument updateGroup:self description:value];
	}
	[properties setObject:value forKey:@"displayDescription"];
	[reportDocument updateChangeCount:NSChangeDone];
}

#pragma mark "Device Methods"

- (NSMutableDictionary *) deviceItems
{ return deviceItems; }

- (LCBWRepDevice *) locateDeviceItem:(LCEntity *)device
{
	return [deviceItems objectForKey:[[device entityAddress] addressString]];	
}

- (void) addDeviceItem:(LCBWRepDevice *)item
{
	[deviceItems setObject:item forKey:[[[item entity] entityAddress] addressString]];
}

- (void) removeDeviceItem:(LCBWRepDevice *)item
{
	[deviceItems removeObjectForKey:[[[item entity] entityAddress] addressString]];
}

#pragma mark "Group Removal"

- (void) removeGroup
{
	/* Remove group and all children items */
	NSEnumerator *childEnum = [[self children] objectEnumerator];
	LCBWRepItem *item;
	while (item=[childEnum nextObject])
	{
		if ([item type] == BWREP_ITEM_DEVICE)
		{
			LCBWRepDevice *dev = (LCBWRepDevice *) item;
			[dev removeDevice];
		}
		else if ([item type] == BWREP_ITEM_GROUP)
		{
			LCBWRepGroup *group = (LCBWRepGroup *) item;
			[group removeGroup];
		}
	}
	
	/* Remove from parent */
	if (parent)
	{
		[parent removeObjectFromChildrenAtIndex:[[parent children] indexOfObject:self]];
	}
	else
	{
		[reportDocument removeObjectFromItemsAtIndex:[[reportDocument items] indexOfObject:self]];
	}
	
	/* Remove from document dictionary */
	[reportDocument removeGroupItem:self];
}

@synthesize deviceItems;
@end
