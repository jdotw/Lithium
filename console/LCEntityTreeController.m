//
//  LCEntityTreeController.m
//  Lithium Console
//
//  Created by James Wilson on 1/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityTreeController.h"

#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCMetricGraphController.h"

@implementation LCEntityTreeController

#pragma mark "NIB Methods"

- (void)awakeFromNib
{
	[outlineView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", nil]];
	[super awakeFromNib];
}

#pragma mark "Drag and Drop Methods"

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index
{
	return NO;
}

- (NSDragOperation)outlineView:(NSOutlineView *)outlineView 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index
{
	return NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard
{
	NSMutableArray *propertyList;
	NSMutableArray *graphPropertyList = nil;
	NSMutableArray *graphEntityList;

	/* Create paste-data property list */
	propertyList = [NSMutableArray array];

	/* Loop through each selected entity */
	graphEntityList = [NSMutableArray array];
	id item;
	for (item in items)
	{		
		/* Create entity descriptor */
		LCEntity *entity = [item representedObject];
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
		
		/* Create dup properties dictionary */
		NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
		
		/* Add properties to list */
		[propertyList addObject:properties];
		
		/* Get graphable entities */
		[graphEntityList addObjectsFromArray:[LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:entity]]];
	}
	
	/* Check for graphable entities */
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
	if (graphPropertyList)
	{
		[pboard declareTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor_Graph", @"LCEntityDescriptor", nil] owner:self];
		[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];
		[pboard setPropertyList:graphPropertyList forType:@"LCEntityDescriptor_Graph"];
	}
	else
	{		
		[pboard declareTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"] owner:self];
		[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];
	}
	
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


@synthesize outlineView;
@end
