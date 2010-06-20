//
//  LCBWRepDevice.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepDevice.h"
#import "LCBWRepGroup.h"

@implementation LCBWRepDevice

#pragma mark "Constructors"

+ (LCBWRepDevice *) deviceItemWithEntity:(LCEntity *)initEntity
{
	return [[[LCBWRepDevice alloc] initWithEntity:initEntity] autorelease];
}

- (LCBWRepDevice *) initWithEntity:(LCEntity *)initEntity
{
	[super initWithEntity:initEntity];
	[self setType:BWREP_ITEM_DEVICE];
	return self;
}

- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];	
    return self;
}

#pragma mark "Interface Methods"

- (void) removeInterface:(LCBWRepInterface *)iface
{
	/* Remove the interface from children list and
	 * from the report document. If the device is then
	 * empty, remove it from the parent group/document
	 * and relevant dictionaries
	 */
	
	/* Remove from report dictionary */
	[reportDocument removeInterfaceItem:iface];
	
	/* Remove from children list */
	[self removeObjectFromChildrenAtIndex:[[self children] indexOfObject:iface]];
	
	/* Check to see if we're now empty */
	if ([[self children] count] < 1)
	{
		/* Remove ourselves as we're now an empty device */
		if ([self parent])
		{
			/* Remove from our group */
			LCBWRepGroup *parentGroup = [self parent];
			[parentGroup removeDeviceItem:self];
			[parentGroup removeObjectFromChildrenAtIndex:[[parentGroup children] indexOfObject:self]];
		}
		else
		{
			/* Remove from root of document */
			[reportDocument removeDeviceItem:self];
			[reportDocument removeObjectFromItemsAtIndex:[[reportDocument items] indexOfObject:self]];
		}
	}	
}

#pragma mark "Device methods"

- (void) removeDevice
{
	/* Remove all interfaces. In the process this 
	 * will remove the devices.
	 */
	NSEnumerator *ifaceEnum = [[self children] objectEnumerator];
	LCBWRepInterface *iface;
	while (iface=[ifaceEnum nextObject])
	{
		[self removeInterface:iface];
	}
}

#pragma mark "Accessors"

- (NSString *) displayDescription
{ return [[self entity] displayString]; }

@end
