//
//  MBOIDTreeController.m
//  ModuleBuilder
//
//  Created by James Wilson on 25/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBOIDTreeController.h"

#import "MBOIDCell.h"


@implementation MBOIDTreeController

#pragma mark NIB Methods

- (void)awakeFromNib
{
    [outlineView registerForDraggedTypes:[NSArray arrayWithObjects:@"MBOidSection", @"MBOid", nil]];
	[super awakeFromNib];
}

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)outlineView:(NSOutlineView *)ov 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index
{	
	return NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)ov 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index
{
	return NO;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard
{
	/* Create paste-data property lists */
	NSMutableArray *oidPropertyList = [NSMutableArray array];
	NSMutableArray *sectionPropertyList = [NSMutableArray array];
	
	/* Loop through each selected entity */
	NSEnumerator *itemEnumerator = [items objectEnumerator];
	id item;
	while (item = [itemEnumerator nextObject])
	{		
		/* Create entity descriptor */
		id obj = [item representedObject];
		if ([obj class] == [MBOidSection class])
		{
			/* Section */
			MBOidSection *section = (MBOidSection *) obj;
			[sectionPropertyList addObject:[NSKeyedArchiver archivedDataWithRootObject:section]];
		}
		else if ([obj class] == [MBOid class])
		{
			/* OID */
			MBOid *oid = (MBOid *) obj;
			[oidPropertyList addObject:[NSKeyedArchiver archivedDataWithRootObject:oid]];
		}
	}
	
	if ([sectionPropertyList count] > 0) 
	{
		[pboard declareTypes:[NSArray arrayWithObjects:@"MBOidSection", nil] owner:nil];
		[pboard setPropertyList:sectionPropertyList forType:@"MBOidSection"];
	}
	else if ([oidPropertyList count] > 0) 
	{
		[pboard declareTypes:[NSArray arrayWithObjects:@"MBOid", nil] owner:nil];
		[pboard setPropertyList:oidPropertyList forType:@"MBOid"];
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

- (void)outlineView:(NSOutlineView *)outlineView 
	willDisplayCell:(id)cell 
	 forTableColumn:(NSTableColumn *)tableColumn 
			   item:(id)item
{
	MBOIDCell *oidCell = cell;
	if ([oidCell class] == [MBOIDCell class])
	{
		if ([[item representedObject] class] == [MBOid class])
		{ oidCell.drawnOid = [item representedObject]; }
		else
		{ oidCell.drawnOid = nil; }
	}
}


@end
