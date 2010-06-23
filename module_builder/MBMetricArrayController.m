//
//  MBMetricArrayController.m
//  ModuleBuilder
//
//  Created by James Wilson on 17/08/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBMetricArrayController.h"
#import "MBMetric.h"

@implementation MBMetricArrayController

#pragma mark Drag and Drop Methods

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	return NSDragOperationNone;
}

- (BOOL)tableView:(NSTableView*)tv
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row
	dropOperation:(NSTableViewDropOperation)op
{
	return NO;
}

- (BOOL)tableView:(NSTableView *)tv
		writeRows:(NSArray*)rows
	 toPasteboard:(NSPasteboard*)pboard
{
	/* Create paste-data property list */
	NSMutableArray *propertyList = [NSMutableArray array];
	NSEnumerator *rowEnumerator = [rows objectEnumerator];
	NSNumber *index;
	while (index = [rowEnumerator nextObject])
	{		
		MBMetric *metric = [[self arrangedObjects] objectAtIndex:[index intValue]];
		NSDictionary *dict = [NSDictionary dictionaryWithObject:[metric desc] forKey:@"desc"];
		[propertyList addObject:dict];
	}
	[pboard declareTypes:[NSArray arrayWithObject:@"MBMetric"] owner:nil];
	[pboard setPropertyList:propertyList forType:@"MBMetric"];
	
	return YES;
}

#pragma mark "TableView Datasource Methods"

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{ return [[self arrangedObjects] count]; }
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{ return nil; }

@end
