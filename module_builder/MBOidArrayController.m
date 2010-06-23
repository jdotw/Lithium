//
//  MBOidArrayController.m
//  ModuleBuilder
//
//  Created by James Wilson on 14/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBOidArrayController.h"
#import "MBOid.h"
#import "MBOIDCell.h"

@implementation MBOidArrayController

#pragma mark NIB Methods

- (void)awakeFromNib
{
    [tableView registerForDraggedTypes:[NSArray arrayWithObjects:@"MBOid", nil]];
	[super awakeFromNib];
}

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
		MBOid *oid = [[self arrangedObjects] objectAtIndex:[index intValue]];
		[propertyList addObject:[NSKeyedArchiver archivedDataWithRootObject:oid]];
	}
	
	[pboard declareTypes:[NSArray arrayWithObject:@"MBOid"] owner:self];
	[pboard setPropertyList:propertyList forType:@"MBOid"];
	
	return YES;
}

#pragma mark "TableView Delegate Methods"

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
}

#pragma mark "TableView Datasource Methods"

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{ return [[self arrangedObjects] count]; }
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)rowIndex
{ return nil; }

@end
