//
//  LCDeviceTreeOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 4/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDeviceTreeOutlineView.h"

#import "LCDeviceTreeItem.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCDevice.h"
#import "LCIncident.h"
#import "LCDeviceTreeIncidentItem.h"
#import "LCDeviceTreeDeviceItem.h"
#import "LCDeviceTreeCell.h"

@implementation LCDeviceTreeOutlineView

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

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(LCDeviceTreeCell *)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	[cell setRepresentedObject:[item representedObject]];
	
	/* Check for last object */
	if ([self levelForItem:item] == 1)
	{
		/* Check for first/last entity */
		LCDeviceTreeItem *treeItem = [[self parentForItem:item] representedObject];
		LCIncident *incident = [item representedObject];
		if ([treeItem.children indexOfObject:incident] == (treeItem.children.count - 1))
		{
			/* Last incident */
			cell.lastRow = YES;
		}
		else
		{ cell.lastRow = NO; }
		if ([treeItem.children indexOfObject:incident] == 0)
		{
			/* First incident */
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
	[super selectRowIndexes:indexes byExtendingSelection:extend];
	[self setNeedsDisplay:YES];
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
		LCEntity *entity = nil;
		if ([[obj class] isSubclassOfClass:[LCEntity class]])
		{		
			entity = [item representedObject];
		}
		else if ([[obj class] isSubclassOfClass:[LCIncident class]])
		{
			entity = [(LCIncident *)[item representedObject] entity];
		}

		/* Create entity descriptor */
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
		
		/* Create dup properties dictionary */
		NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
		
		/* Add properties to list */
		[propertyList addObject:properties];
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

#pragma mark "Expansion"

- (BOOL) expandByDefault
{ return NO; }

@end
