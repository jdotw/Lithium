//
//  MBContainerTreeController.m
//  ModuleBuilder
//
//  Created by James Wilson on 14/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBContainerTreeController.h"
#import "MBOid.h"
#import "MBEntity.h"
#import "MBContainer.h"
#import "MBMetric.h"

@implementation MBContainerTreeController

#pragma mark "NIB Methods"

- (void)awakeFromNib
{
	[outlineView registerForDraggedTypes:[NSArray arrayWithObjects:@"MBOidSection", @"MBOid", @"MBEntity", nil]];
	[super awakeFromNib];
}

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)outlineView:(NSOutlineView *)ov 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index
{	
	/* Check for move */
	if ([info draggingSource] == outlineView)
	{ 
		NSArray *entityPropertiesArray = [[info draggingPasteboard] propertyListForType:@"MBEntity"];
		for (NSDictionary *entityProperties in entityPropertiesArray)
		{
			int type = [[entityProperties objectForKey:@"type"] intValue];
			MBContainer *container = nil;
			MBMetric *metric = nil;
			MBTrigger *trigger = nil;
			if (type >= 4) container = [document containerNamed:[entityProperties objectForKey:@"container"]];
			if (type >= 6) metric = (MBMetric *) [container childNamed:[entityProperties objectForKey:@"metric"]];
			if (type >= 7) trigger = (MBTrigger *) [metric childNamed:[entityProperties objectForKey:@"trigger"]];
			
			if (trigger)
			{
				/* Drag and drop trigger (not supported) */
				return NSDragOperationNone;
			}
			else if (metric)
			{
				/* Drag and drop metric (move) */
				if (item && [outlineView levelForItem:item] == 0 && index >= 0)
				{
					/* Drop on to container to move metric */
					MBContainer *destCont = (MBContainer *) [item representedObject];
					if (destCont == [metric parent]) return NSDragOperationMove;
					else return NSDragOperationNone;
				}
				else
				{
					/* Drop on non-item or non-container not supported */
					return NSDragOperationNone;
				}
			}
			else if (container)
			{
				/* Drag and drop container (move) */
				if (item && index >= 0)
				{
					/* Drop on non-root (not allowed) */
					return NSDragOperationNone;
				}
				else
				{
					/* Drop on to root to re-order container */
					return NSDragOperationMove;
				}
			}
		}
	}
	
	/* Check level */
	if ([outlineView levelForItem:item] > 0) 
	{ return NSDragOperationNone; }

	return NSDragOperationCopy;
}

- (BOOL)outlineView:(NSOutlineView *)ov 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index
{
	/* Get Entity Descriptors */
	NSArray *sectionPropertiesArray = [[info draggingPasteboard] propertyListForType:@"MBOidSection"];
	NSArray *oidPropertiesArray = [[info draggingPasteboard] propertyListForType:@"MBOid"];
	NSArray *entityPropertiesArray = [[info draggingPasteboard] propertyListForType:@"MBEntity"];
	
	if ([info draggingSource] != outlineView)
	{
		for (NSData *data in sectionPropertiesArray)
		{
			/* Create Section Object */
			MBOidSection *section = [NSKeyedUnarchiver unarchiveObjectWithData:data];
			
			/* Check drop level */
			if (!item)
			{
				/* Adding new container */
				MBContainer *container;
				BOOL dynamicContainer;
				if([[[section.children objectAtIndex:0] children] count] == 1 && !strstr([[[section.children objectAtIndex:0] name] cStringUsingEncoding:NSUTF8StringEncoding], "Index"))
				{
					/* Single-object container */
					container = [MBContainer container];
					MBOid *oid = [section.children objectAtIndex:0];
					if (oid.commonPrefix) [container setDesc:oid.commonPrefix];
					else [container setDesc:section.name];
					dynamicContainer = NO;
				}
				else
				{
					/* Multi-object container */
					container = [MBContainer containerWithIndexOid:[section.children objectAtIndex:0]];
					dynamicContainer = YES;
				}
				
				if (index <= [document countOfContainers])
				{ [document insertObject:container inContainersAtIndex:index]; }
				else
				{ [document insertObject:container inContainersAtIndex:[document countOfContainers]]; }
				
				/* Add Section metrics */
				int childIndex = 0;
				for (MBOid *oid in section.children)
				{
					if (childIndex > 0 || !dynamicContainer)
					{
						MBMetric *metric = [MBMetric metricWithOid:oid];
						[container insertObject:metric inChildrenAtIndex:[container countOfChildren]];
					}
					childIndex++;
				}
			}
			else
			{
				/* Unsupported drag */
				return NO;
			}
		}
		
		for (NSData *data in oidPropertiesArray)
		{
			/* Create OID Object */
			MBOid *oid = [NSKeyedUnarchiver unarchiveObjectWithData:data];;
			
			/* Check level */
			if (item && [outlineView levelForItem:item] == 0)
			{
				/* Dropping an OID on to a container, create new metric */
				MBContainer *container = (MBContainer *) [item representedObject];
				MBMetric *metric = [MBMetric metricWithOid:oid];
				if (index <= [container countOfChildren])
				{ [container insertObject:metric inChildrenAtIndex:index]; }
				else
				{ [container insertObject:metric inChildrenAtIndex:[container countOfChildren]]; }
			}
			else if (item && [outlineView levelForItem:item] == 1)
			{
				/* Dropping an OID on to an existing metric, update OID */
				MBMetric *metric = (MBMetric *) [item representedObject];
				[metric setOid:oid];
			}
			else if (!item)
			{
				/* No item, add new container */
				MBContainer *container = [MBContainer containerWithIndexOid:oid];
				if (index <= [document countOfContainers])
				{ [document insertObject:container inContainersAtIndex:index]; }
				else
				{ [document insertObject:container inContainersAtIndex:[document countOfContainers]]; }				
			}
			else
			{
				/* Unsupported drag */
				return NO;
			}
		}
	}
	else 
	{
		for (NSDictionary *entityProperties in entityPropertiesArray)
		{
			int type = [[entityProperties objectForKey:@"type"] intValue];
			MBContainer *container = nil;
			MBMetric *metric = nil;
			MBTrigger *trigger = nil;
			if (type >= 4) container = [document containerNamed:[entityProperties objectForKey:@"container"]];
			if (type >= 6) metric = (MBMetric *) [container childNamed:[entityProperties objectForKey:@"metric"]];
			if (type >= 7) trigger = (MBTrigger *) [metric childNamed:[entityProperties objectForKey:@"trigger"]];
			
			if (metric)
			{
				/* Drag and drop metric on to container (move) */
				MBContainer *destCont = (MBContainer *) [item representedObject];
				if ([info draggingSourceOperationMask] & NSDragOperationGeneric)
				{
					/* Move */
					[destCont moveChild:metric toIndex:index];
				}
			}
			else if (container)
			{
				/* Drag and drop container (move) */
				[document moveContainer:container toIndex:index];
			}
		}
	}	
		
	return YES;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard
{
	/* Create paste-data property lists */
	NSMutableArray *propertyList = [NSMutableArray array];
	NSMutableArray *oidPropertyList = [NSMutableArray array];
	
	/* Loop through each selected entity */
	NSEnumerator *itemEnumerator = [items objectEnumerator];
	id item;
	while (item = [itemEnumerator nextObject])
	{		
		/* Create entity descriptor */
		MBEntity *entity = (MBEntity *) [item representedObject];
		NSMutableDictionary *properties = [NSMutableDictionary dictionary];
		[properties setObject:[entity type] forKey:@"type"];
		switch ([[entity type] intValue])
		{
			case 4:
				[properties setObject:[entity desc] forKey:@"container"];
				break;
			case 6:
				[properties setObject:[[entity parent] desc] forKey:@"container"];
				[properties setObject:[entity desc] forKey:@"metric"];
				[oidPropertyList addObject:[NSKeyedArchiver archivedDataWithRootObject:[(MBMetric *)entity oid]]];
				break;
			case 7:
				[properties setObject:[[[entity parent] parent] desc] forKey:@"container"];
				[properties setObject:[[entity parent] desc] forKey:@"metric"];
				[properties setObject:[entity desc] forKey:@"trigger"];
				break;
		}
		[propertyList addObject:properties];
	}
	
	NSMutableArray *types = [NSMutableArray arrayWithObject:@"MBEntity"];
	if ([oidPropertyList count]) [types addObject:@"MBOid"];
	
	[pboard declareTypes:types owner:self];
	[pboard setPropertyList:propertyList forType:@"MBEntity"];
	if ([oidPropertyList count]) [pboard setPropertyList:oidPropertyList forType:@"MBOid"];
	
	return YES;
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
	
}

#pragma mark "Accessors"

- (ModuleDocument *) document
{ return document; }

- (void) setDocument:(ModuleDocument *)newDocument
{ document = newDocument; }

@end
