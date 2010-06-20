//
//  LCMetricGraphEntityArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 3/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMetricGraphEntityArrayController.h"
#import "LCXMLRequest.h"
#import "LCEntityDescriptor.h"

@implementation LCMetricGraphEntityArrayController

#pragma mark "Tableview Data Source Methods"

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	/* Return coloured square for metrics */
	
	if (tableColumn == colourColumn)
	{
		switch (rowIndex)
		{
			case 0: return [NSImage imageNamed:@"Blue1.tif"];
			case 1: return [NSImage imageNamed:@"Green1.tif"];
			case 2: return [NSImage imageNamed:@"Red1.tif"];
			case 3: return [NSImage imageNamed:@"Purple1.tif"];
			case 4: return [NSImage imageNamed:@"Yellow1.tif"];
			case 5: return [NSImage imageNamed:@"Cyan1.tif"];
			case 6: return [NSImage imageNamed:@"Orange1.tif"];
			case 7: return [NSImage imageNamed:@"Pink1.tif"];
			default: return nil;
		}
	}
	
	return nil;
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{ return [[self arrangedObjects] count]; }

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor_Graph"];
	if (!propertiesArray)
	{
		propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	}
	
	/* Process each descriptor */
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		/* Create descriptor and locate entity */
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
		if ([[entityDesc type_num] intValue] < 6) 
		{
			return NSDragOperationNone;
		}
	}
	
	return NSDragOperationCopy;
}

- (BOOL)tableView:(NSTableView*)tv
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row
	dropOperation:(NSTableViewDropOperation)op
{
	/* If the entity is a trigger, add the triggers 
	 * metric parent to the graph
	 */
	
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor_Graph"];
	if (!propertiesArray)
	{
		/* Nothing found for LCEntityDescriptor_Graph type, try LCEntityDescriptor_Graph */
		propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	}
	
	/* Process each descriptor */
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		/* Create descriptor and locate entity */
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
		LCEntity *entity = [entityDesc locateEntity:YES];

		/* Check for triggers */
		if ([[entity typeInteger] intValue] == 7)
		{
			/* Entity is a trigger, use the metric it belongs to */
			entity = [entity metric];
		}
		
		/* Create item */
		LCMetricGraphMetricItem *item = [LCMetricGraphMetricItem new];
		item.metric = (LCMetric *) entity;

		/* Add entity to list */
		[self insertObject:item atArrangedObjectIndex:row];
	}
	
	return YES;
}

#pragma mark "Array accessor methods"

- (void) insertObject:(id)object atArrangedObjectIndex:(unsigned int)index
{
	[super insertObject:object atArrangedObjectIndex:index];
	if (graphController) [graphController refreshGraph:XMLREQ_PRIO_HIGH];
}

- (void) removeObjectsAtArrangedObjectIndexes:(NSIndexSet *)indexes
{
	[super removeObjectsAtArrangedObjectIndexes:indexes];
	if (graphController) 
	{ [graphController refreshGraph:XMLREQ_PRIO_HIGH]; }
	if (graphController && [[self arrangedObjects] count] < 1)
	{ [graphController blankGraph]; }
}

@synthesize colourColumn;
@synthesize graphController;
@end
