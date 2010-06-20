//
//  LCContainerTreeOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 27/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCContainerTreeOutlineView.h"

#import "LCContainerTreeItem.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCContainer.h"
#import "LCContainerTreeDeviceItem.h"

@implementation LCContainerTreeOutlineView

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	/* Set-up outline view */
	[super awakeFromNib];
	[self registerForDraggedTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"]];
	[self setDelegate:self];
	[self setDataSource:self];
	[self setIntercellSpacing:NSMakeSize(2.0, 2.0)];
}

#pragma mark "Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(LCContainerTreeCell *)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	[cell setRepresentedObject:[item representedObject]];
	
	/* Check for last object */
	if ([self levelForItem:item] == 1)
	{
		/* Check for first/last entity */
		LCContainerTreeItem *treeItem = [[self parentForItem:item] representedObject];
		LCEntity *entity = [item representedObject];
		if ([treeItem.children indexOfObject:entity] == (treeItem.children.count - 1))
		{
			/* Last container */
			cell.lastRow = YES;
		}
		else
		{ cell.lastRow = NO; }
		if ([treeItem.children indexOfObject:entity] == 0)
		{
			/* First container */
			cell.firstRow = YES;
		}
		else
		{ cell.firstRow = NO; }
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	if ([self levelForRow:[self rowForItem:item]] == 0) return 22.0;
	else return 13.0;
}

- (void)outlineViewItemWillExpand:(NSNotification *)notification
{
}

- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
}

- (void)outlineViewItemWillCollapse:(NSNotification *)notification
{
}

- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
}

#pragma mark "Selection Management"

- (void) selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend
{
	LCContainerTreeItem *treeItem = [[self itemAtRow:[indexes firstIndex]] representedObject];
	if ([self levelForRow:[indexes firstIndex]] == 0 || ![treeItem selectable])
	{ 
		/* Do not allow selection of root items */
		return;
	}
	[super selectRowIndexes:indexes byExtendingSelection:extend];
}

- (void) selectContainer:(LCContainer *)container
{
	int i;
	
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		LCEntity *rowEntity = [item representedObject];
		
		if (![[rowEntity class] isSubclassOfClass:[LCEntity class]]) continue;
		
		if (rowEntity == container)
		{
			/* Match */
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
			[self scrollRowToVisible:i];
			return;
		}
		else if ([container isDescendantOf:rowEntity])
		{
			/* Parent */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
	}
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
	
	/* Create paste-data property list */
	propertyList = [NSMutableArray array];
	
	/* Loop through each selected entity */
	id item;
	for (item in items)
	{	
		/* Get representedObject */
		id obj = [item representedObject];
		if ([[obj class] isSubclassOfClass:[LCEntity class]])
		{		
			/* Create entity descriptor */
			LCEntity *entity = [item representedObject];
			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
			
			/* Create dup properties dictionary */
			NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
			
			/* Add properties to list */
			[propertyList addObject:properties];
		}
	}
	
	[pboard declareTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"] owner:self];
	[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];
	
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


@end
