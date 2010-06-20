//
//  LCBWRepItemTreeController.m
//  Lithium Console
//
//  Created by James Wilson on 26/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepItemTreeController.h"

#import "LCEntity.h"
#import "LCBWRepItem.h"
#import "LCBWRepGroup.h"
#import "LCBWRepDevice.h"
#import "LCBWRepInterface.h"
#import "LCEntityDescriptor.h"
#import "LCMetricGraphController.h"

@implementation LCBWRepItemTreeController

#pragma mark "NIB Methods"

- (void)awakeFromNib
{
	[outlineView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", @"LCBWRepGroup", nil]];
	
	/* Set browser cell */
	id cell = [[NSBrowserCell alloc] init];
	[cell setFont:[NSFont fontWithDescriptor:[[cell font] fontDescriptor] size:10.0]];
	[descColumn setDataCell:cell];
	[cell release];
	
	/* Call super */
	[super awakeFromNib];
}

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)outlineView:(NSOutlineView *)ov 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index
{
	/* This returns NSDragOperationCopy or NSDragOperationMove
	 * if there is AT LEAST ONE valid item
	 */
	LCBWRepItem *dropItem = [item representedObject];
	
	/* Validate drop item, MUST ONLY BE A GROUP */
	if (dropItem && [dropItem type] != BWREP_ITEM_GROUP)
	{
		return NSDragOperationNone;
	}
	
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	if (propertiesArray)
	{
		/* This is an entity being dropped
		 * Process each descriptor
		 */
		NSDictionary *properties;
		NSMutableArray *dropIfaces = [NSMutableArray array];
		for (properties in propertiesArray)
		{
			/* Create descriptor and locate entity */
			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
			LCEntity *dropEntity = [entityDesc locateEntity:YES];
			
			/* Get list of interfaces */
			if ([[entityDesc type_num] intValue] == 4)
			{
				/* Container being dragged, add ifaces */
				if([[entityDesc name] isEqualToString:@"iface"])
				{ 
					/* Add all child ifaces */
					[dropIfaces addObjectsFromArray:[dropEntity children]];
				}
			}
			else if ([[entityDesc type_num] intValue] == 5)
			{
				/* Object being dragged, allow is cntname==iface */
				if ([[entityDesc cnt_name] isEqualToString:@"iface"])
				{ 
					/* Add single iface */
					[dropIfaces addObject:dropEntity];
				}
			}
		}
		
		/* Process ifaces */
		LCEntity *dropIface;
		for (dropIface in dropIfaces)
		{
			if ([reportDocument locateInterfaceItem:dropIface])
			{
				/* Iface exists */
				if ([info draggingSource] == outlineView)
				{
					/* It's a move of an existing iface, allow it */
					return NSDragOperationMove; 
				}
			}
			else
			{
				/* Iface does not exist */
				return NSDragOperationCopy;
			}
		}
	}
	else
	{
		/* This must be an item move/drag */
		propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCBWRepGroup"];
		if (propertiesArray) 
		{
			if ((!dropItem || [dropItem type] == BWREP_ITEM_GROUP) && [info draggingSource] == outlineView)
			{
				/* Dragging a group to another group or the root */
				return NSDragOperationMove;
			}
		}
	}	
	
	return NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index
{
	/* Parent item (CAN ONLY EVER BE A GROUP OR NULL) */
	LCBWRepGroup *parentGroup = nil;
	if (item && [item representedObject])
	{ parentGroup = [item representedObject]; }
	
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	if (propertiesArray)
	{
		/* Process each descriptor */
		NSDictionary *properties;
		for (properties in propertiesArray)
		{
			/* Create descriptor and locate entity */
			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
			LCEntity *entity = [entityDesc locateEntity:YES];
			
			/* Check to see if this iface exists */
			if ([reportDocument locateInterfaceItem:entity])
			{
				/* Iface exists, remove the current entry for it..
				 * then proceed to re-adding it as if it were a new iface
				 */
				LCBWRepInterface *existingItem = [reportDocument locateInterfaceItem:entity];
				LCBWRepDevice *existingDevice = [existingItem parent];
				[existingDevice removeInterface:existingItem];
			}
				
			/* Locate device */
			LCEntity *device = [entity device];
			LCBWRepDevice *deviceItem;
			if (parentGroup)
			{ 
				deviceItem = [parentGroup locateDeviceItem:device];
			}
			else
			{
				deviceItem = [reportDocument locateDeviceItem:device];
			}
			if (!deviceItem)
			{
				/* Create new device item */
				deviceItem = [LCBWRepDevice deviceItemWithEntity:device];
				if (parentGroup)
				{ 
					[parentGroup addDeviceItem:deviceItem];
					[parentGroup insertObject:deviceItem inChildrenAtIndex:[parentGroup countOfChildren]]; 
				}
				else
				{ 
					[reportDocument addDeviceItem:deviceItem];
					[reportDocument insertObject:deviceItem inItemsAtIndex:[reportDocument countOfItems]]; 
				}
			}
		
			/* Check for type */
			NSMutableArray *interfaces;
			if ([[entity typeInteger] intValue] == 4)
			{
				/* Adding a device/container and all its interfaces */
				interfaces = [NSMutableArray arrayWithArray:[entity children]];			
			}
			else if ([[entity typeInteger] intValue] == 5)
			{
				/* Adding one interface */
				interfaces = [NSMutableArray arrayWithObject:entity];
			}
			
			/* Add entity to list */
			NSEnumerator *ifaceEnum = [interfaces objectEnumerator];
			LCEntity *iface;
			while (iface=[ifaceEnum nextObject])
			{
				/* Check to see if interface already exists */
				if ([reportDocument locateInterfaceItem:iface])
				{ continue; }
				
				/* Create new iface item */
				LCBWRepInterface *ifaceItem = [LCBWRepInterface interfaceItemWithEntity:iface];
				[reportDocument addInterfaceItem:ifaceItem];
				[deviceItem insertObject:ifaceItem inChildrenAtIndex:[deviceItem countOfChildren]];
			}
		}
	}
	else
	{
		/* Nothing found for LCEntityDescriptor type, try LCBWRepGroup */
		propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCBWRepGroup"];
		if (propertiesArray)
		{
			/* Process a move/re-order of a LCBWRepGroup */
			NSMutableDictionary *itemProperties;
			for (itemProperties in propertiesArray)
			{
				LCBWRepItem *group = [reportDocument locateGroupItem:[itemProperties objectForKey:@"displayDescription"]];
				[group moveToParent:parentGroup];
			}
		}
		else 
		{ return NO; }
	}
	
	
	return YES;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard
{
	
	/* Create paste-data property lists */
	NSMutableArray *propertyList = [NSMutableArray array];
	NSMutableArray *itemPropertyList = [NSMutableArray array];	
	
	/* Loop through each selected entity */
	NSMutableArray *graphEntityList = [NSMutableArray array];
	id item;
	for (item in items)
	{		
		/* Create entity descriptor */
		LCBWRepItem *reportItem = (LCBWRepItem *)[item representedObject];
		
		/* Check item type */
		if ([reportItem type] == BWREP_ITEM_DEVICE || [reportItem type] == BWREP_ITEM_INTERFACE)
		{
			/* Create list of ifaces to be written */
			NSMutableArray *ifaceItemList = [NSMutableArray array];
			if ([reportItem type] == BWREP_ITEM_DEVICE)
			{ 
				[ifaceItemList addObjectsFromArray:[reportItem children]]; 
			}
			else
			{ [ifaceItemList addObject:reportItem]; }
			
			/* Add properties for interfaces */
			NSEnumerator *ifaceItemEnum = [ifaceItemList objectEnumerator];
			LCBWRepInterface *ifaceItem;
			while (ifaceItem=[ifaceItemEnum nextObject])
			{
				/* Get entity desc */
				LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:[ifaceItem entity]];
				
				/* Create dup properties dictionary */
				NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
				
				/* Add properties to list */
				[propertyList addObject:properties];
				
				/* Add graphable metrics */
				[graphEntityList addObjectsFromArray:[LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:[ifaceItem entity]]]];
			}
		}
		else if ([reportItem type] == BWREP_ITEM_GROUP)
		{
			/* If a group item, Add item properties */
			NSDictionary *groupProperties = [NSDictionary dictionaryWithObject:[reportItem displayDescription] forKey:@"displayDescription"];
			[itemPropertyList addObject:groupProperties];
		}
	}
	
	/* Check for graphable entities */
	NSMutableArray *graphPropertyList = nil;
	if ([graphEntityList count] > 0)
	{
		/* Create property list */
		graphPropertyList = [NSMutableArray array];
		
		/* Loop through each graphable entity */
		LCEntity *entity;
		for (entity in graphEntityList)
		{
			/* Create properties for entity and add it to the array */
			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
			NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
			[graphPropertyList addObject:properties];
		}
	}
	
	/* Declare types and set paste data */
	NSMutableArray *typesArray = [NSMutableArray array];
	if (propertyList && [propertyList count] > 0)
	{
		[typesArray addObject:@"LCEntityDescriptor"];
	}
	if (graphPropertyList && [graphPropertyList count] > 0)
	{
		[typesArray addObject:@"LCEntityDescriptor_Graph"];
	}
	if (itemPropertyList && [itemPropertyList count] > 0)
	{
		[typesArray addObject:@"LCBWRepGroup"];
	}
	[pboard declareTypes:typesArray owner:self];
	if (propertyList && [propertyList count] > 0)
	{
		[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];
	}
	if (graphPropertyList && [graphPropertyList count] > 0)
	{
		[pboard setPropertyList:graphPropertyList forType:@"LCEntityDescriptor_Graph"];
	}
	if (itemPropertyList && [itemPropertyList count] > 0)
	{
		[pboard setPropertyList:itemPropertyList forType:@"LCBWRepGroup"];
	}
	
	return YES;
}

#pragma mark Menu Items

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	if ([[self selectedObjects] count] > 0)
	{
		LCBWRepItem *selectedItem = [[self selectedObjects] objectAtIndex:0];
		if ([selectedItem type] == BWREP_ITEM_GROUP)
		{
			LCBWRepGroup *group = (LCBWRepGroup *) selectedItem;
			if ([group arrangeByDevice])
			{ [arrangeByDeviceMenuItem setState:1]; }
			else
			{ [arrangeByDeviceMenuItem setState:0]; }
			return groupMenu; 
		}
		else if ([selectedItem type] == BWREP_ITEM_DEVICE)
		{ return deviceMenu; }
		else if ([selectedItem type] == BWREP_ITEM_INTERFACE)
		{ return ifaceMenu; }
		else
		{ return nil; }
	}
	else
	{ return nil; }
}

#pragma mark NSOutlineView Hacks for Pretending to be a data source

- (BOOL) outlineView: (NSOutlineView *)ov
	isItemExpandable: (id)item { return NO; }

- (int)  outlineView: (NSOutlineView *)ov
         numberOfChildrenOfItem:(id)item { return 0; }

- (id)   outlineView: (NSOutlineView *)ov
			   child:(int)index
			  ofItem:(id)item { return nil; }

- (id)   outlineView: (NSOutlineView *)ov
         objectValueForTableColumn:(NSTableColumn*)col
			  byItem:(id)item { return nil; }

- (void)outlineView:(NSOutlineView *)outlineView 
	willDisplayCell:(id)cell 
	 forTableColumn:(NSTableColumn *)tableColumn 
			   item:(id)item
{
	if (tableColumn == descColumn)
	{ 
		NSBrowserCell *browserCell = cell;
		LCBWRepItem *reportItem = [item representedObject];
		switch ([reportItem type])
		{
			case BWREP_ITEM_GROUP:
				[browserCell setImage:[NSImage imageNamed:@"Group13px.tiff"]]; 
				break;
			case BWREP_ITEM_DEVICE:
				[browserCell setImage:[NSImage imageNamed:@"Device13px.tiff"]]; 
				break;
			case BWREP_ITEM_INTERFACE:
				[browserCell setImage:[NSImage imageNamed:@"Interface13px.tiff"]]; 
		}
		[browserCell setLeaf:YES];
	}
}

#pragma mark Accessors

- (LCBWRepDocument *) reportDocument
{ return reportDocument; }

- (void) setReportDocument:(LCBWRepDocument *)value
{
	[reportDocument release];
	reportDocument = [value retain];
}


@synthesize outlineView;
@synthesize groupMenu;
@synthesize deviceMenu;
@synthesize ifaceMenu;
@synthesize arrangeByDeviceMenuItem;
@synthesize descColumn;
@end
