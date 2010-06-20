//
//  LCEntityArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 3/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCEntityArrayController.h"
#import "LCEntityDescriptor.h"
#import "LCEntity.h"
#import "LCMetricGraphController.h"

@implementation LCEntityArrayController

#pragma mark NIB Methods

- (void)awakeFromNib
{
    [tableView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", nil]];
	[super awakeFromNib];
}

#pragma mark Drag and Drop Methods

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	/* Check config */
	if (allowDrop == NO)
	{ return NSDragOperationNone; }
	
	/* Check for a move operations */
	if ([info draggingSource] == tableView)
	{
		/* Do not allow move */
		return NSDragOperationNone; 
	}
	
	/* Config drop location */
	if (row >= 0)
	{ [tv setDropRow:row dropOperation:NSTableViewDropAbove]; }
	else
	{ [tv setDropRow:0 dropOperation:NSTableViewDropAbove]; }
	
    return dragOp;
}

- (BOOL)tableView:(NSTableView*)tv
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row
	dropOperation:(NSTableViewDropOperation)op
{
	/* Add pasted rows */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:properties];
		LCEntity *entity = [entityDescriptor locateEntity:YES];
		[self insertObject:entity atArrangedObjectIndex:row];
	}
	
	return YES;
}

- (BOOL)tableView:(NSTableView *)tv
		writeRows:(NSArray*)rows
	 toPasteboard:(NSPasteboard*)pboard
{
	NSMutableArray *propertyList;
	NSMutableArray *graphPropertyList = nil;
	NSMutableArray *graphEntityList;
	
	/* Create paste-data property list */
	propertyList = [NSMutableArray array];

	/* Loop through each selected entity */
	graphEntityList = [NSMutableArray array];
	NSNumber *index;
	for (index in rows)
	{		
		/* Create entity descriptor */
		LCMetricGraphMetricItem *metricItem = [[self arrangedObjects] objectAtIndex:[index intValue]];
		LCEntity *entity = (LCEntity *) metricItem.metric;
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

#pragma mark "TableView Datasource Methods"

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{ return [[self arrangedObjects] count]; }
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{ return nil; }

#pragma mark "Accessor Methods"
- (BOOL) allowDrop
{ return allowDrop; }
- (void) setAllowDrop:(BOOL)flag
{ allowDrop = flag; }

@synthesize tableView;
@end
