//
//  LCBWRepItem.m
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepItem.h"
#import "LCBWRepGroup.h"
#import "LCBWRepDocument.h"

@implementation LCBWRepItem

#pragma mark Constructors

- (LCBWRepItem *) init
{
	[super init];
	[self setProperties:[NSMutableDictionary dictionary]];
	[self setChildren:[NSMutableArray array]];
	return self;
}

- (LCBWRepItem *) initWithEntity:(LCEntity *)initEntity
{
	[self init];
	[self setEntity:initEntity];
	return self;
}

- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];	
    return self;
}

- (void) awakeFromArchive:(LCBWRepDocument *)document parent:(LCBWRepItem *)initParent
{
	/* Perform post-unarchive initialisation for this item
	 * and each of its children
	 */

	[self setParent:initParent];
	[self setReportDocument:document];
	if ([self type] == BWREP_ITEM_GROUP)
	{
		/* Register with document */
		[document addGroupItem:(LCBWRepGroup *)self];
	}
	else if ([self type] == BWREP_ITEM_DEVICE)
	{
		/* Device initialisation */
		if ([self parent])
		{ 
			/* Register with parent group */
			LCBWRepGroup *parentGroup = [self parent];
			[parentGroup addDeviceItem:(LCBWRepDevice *)self];
		}
		else
		{
			/* Register with document */
			[document addDeviceItem:(LCBWRepDevice *)self];
		}
	}
	else if ([self type] == BWREP_ITEM_INTERFACE)
	{
		LCBWRepInterface *iface = (LCBWRepInterface *)self;
		[document addInterfaceItem:iface];
		[iface refresh:XMLREQ_PRIO_HIGH];		
	}
	
	/* Initialise children */
	NSEnumerator *childEnum = [[self children] objectEnumerator];
	LCBWRepItem *child;
	while (child=[childEnum nextObject])
	{
		[child awakeFromArchive:document parent:self];
	}
}

#pragma mark "Encoding"

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

#pragma mark Tree Node Methods

- (BOOL) isLeafNode
{
	switch ([self type])
	{
		case BWREP_ITEM_GROUP:
		case BWREP_ITEM_DEVICE:
			if ([[self children] count] > 0)
			{ return NO; }
			else
			{ return YES; }
			break;
		case BWREP_ITEM_INTERFACE:
			return YES;
		default:
			return YES;
	}
}

#pragma mark Properties Methods

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)value
{
	[properties release];
	properties = [value retain];
}

#pragma mark Parent Methods

- (id) parent
{ return parent; }

- (void) setParent:(id)value
{ parent = value; }

- (void) moveToParent:(LCBWRepItem *)newParent
{
	/* Remove from old parent list */
	LCBWRepItem *currentParent = parent;
	if (currentParent)
	{
		[currentParent removeObjectFromChildrenAtIndex:[[currentParent children] indexOfObject:self]];
	}
	else
	{
		[reportDocument removeObjectFromItemsAtIndex:[[reportDocument items] indexOfObject:self]];
	}
	parent = newParent;
	if (newParent)
	{
		[newParent insertObject:self inChildrenAtIndex:[newParent countOfChildren]];
	}
	else
	{
		[reportDocument insertObject:self inItemsAtIndex:[reportDocument countOfItems]];
	}
}

- (id) parentGroup
{
	/* Returns the next parent group 
	 * Will return NULL if there is no group parent
	 */
	LCBWRepItem *parentItem = [self parent];
	while (parentItem && [parentItem type] != BWREP_ITEM_GROUP)
	{
		parentItem = (LCBWRepItem *) [parent parent];
	}
	return parentItem;
}

#pragma mark Type Methods

- (int) type
{
	if ([properties objectForKey:@"type"])
	{ return [[properties objectForKey:@"type"] intValue]; }
	else
	{ return 0; }
}

- (void) setType:(int)value
{
	[properties setObject:[NSNumber numberWithInt:value] forKey:@"type"];
}

#pragma mark Children Methods

- (NSMutableArray *) children
{ return [properties objectForKey:@"children"]; }

- (NSMutableArray *) displayChildren
{ return [self children]; }

- (void) setChildren:(NSMutableArray *)value
{
	[self willChangeValueForKey:@"isLeafNode"];
	[properties setObject:value forKey:@"children"];
	[self didChangeValueForKey:@"isLeafNode"];
}

- (unsigned int) countOfChildren
{ return [[self children] count]; }

- (LCBWRepItem *) objectInChildrenAtIndex:(unsigned int)index
{
	return [[self children] objectAtIndex:index];
}

- (void) insertObject:(LCBWRepItem *)item inChildrenAtIndex:(unsigned int)index
{
	[item setParent:self];
	[item setReportDocument:reportDocument];
	[self willChangeValueForKey:@"isLeafNode"];
	[[self children] insertObject:item atIndex:index];
	[self didChangeValueForKey:@"isLeafNode"];
	[reportDocument updateChangeCount:NSChangeDone];
}

- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	[self willChangeValueForKey:@"isLeafNode"];
	[[self children] removeObjectAtIndex:index];
	[self didChangeValueForKey:@"isLeafNode"];
	[reportDocument updateChangeCount:NSChangeDone];
}

#pragma mark Entity Methods

- (LCEntity *) entity
{ return [properties objectForKey:@"entity"]; }

- (void) setEntity:(LCEntity *)value
{ 
	[properties setObject:value forKey:@"entity"];
}

#pragma mark Placeholder Methods

- (BOOL) canEditDescription
{ return NO; }

- (NSString *) displayDescription
{ return nil; }

- (NSNumber *) inMinimum
{ return nil; }

- (NSNumber *) inAverage
{ return nil; }

- (NSNumber *) inMaximum
{ return nil; }

- (NSNumber *) outMinimum
{ return nil; }

- (NSNumber *) outAverage
{ return nil; }

- (NSNumber *) outMaximum
{ return nil; }

#pragma mark Report Document 

- (id) reportDocument
{ return reportDocument; }

- (void) setReportDocument:(id)doc
{ reportDocument = doc; }

@end
