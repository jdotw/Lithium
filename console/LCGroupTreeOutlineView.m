//
//  LCGroupTreeOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCGroupTreeOutlineView.h"

#import "LCGroupTreeOutlineItem.h"
#import "LCGroupTreeOutlineEntityItem.h"
#import "LCObjectTreeItem.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCObjectTreeMetricViewController.h"
#import "LCMetricGraphController.h"
#import "LCGroupTreeCell.h"

@implementation LCGroupTreeOutlineView

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	/* Set-up outline view */
	displayedSubviews = [[NSMutableArray array] retain];
	[super awakeFromNib];
	self.expansionPreferencePrefix = @"LCGroupTreeOutlineView";
	[self registerForDraggedTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"]];
	[self setDelegate:self];
	[self setDataSource:self];
}

#pragma mark "Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(LCGroupTreeCell *)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	if ([self rowForItem:item] == 0)
	{ cell.firstRow = YES; }
	else
	{ cell.firstRow = NO; }
	
	if ([self rowForItem:item] == ([self numberOfRows] - 1))
	{ cell.lastRow = YES; }
	else
	{ cell.lastRow = NO; }
	
	if ([self isItemExpanded:item])
	{ cell.expanded = YES; }
	else
	{ cell.expanded = NO; }
	
	[cell setRepresentedObject:[item representedObject]];
	
	if ([[item representedObject] class] == [LCObjectTreeMetricItem class])
	{
		LCObjectTreeMetricItem *metricItem = [item representedObject];
		for (LCObjectTreeMetricViewController *viewController in metricItem.displayedMetricViewControllers)
		{
			if (![displayedSubviews containsObject:viewController])
			{ [displayedSubviews addObject:viewController]; }
		}
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	LCObjectTreeItem *treeItem = [item representedObject];
	if (treeItem.rowHeight >= 1.0) return treeItem.rowHeight;
	else return 15.0;
}

- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
	LCObjectTreeHeaderItem *headerItem = [[[notification userInfo] objectForKey:@"NSObject"] representedObject];
	LCObjectTreeMetricItem *metricItem = [headerItem.children objectAtIndex:0];
	for (LCObjectTreeMetricViewController *viewController in metricItem.displayedMetricViewControllers)
	{
		[[viewController view] removeFromSuperviewWithoutNeedingDisplay];
		[displayedSubviews removeObject:viewController];
	}
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
	[self setNeedsDisplay];
}

#pragma mark "Reload Method"

- (void) reloadData
{
	/* Remove all subviews */
	for (LCObjectTreeMetricViewController *viewController in displayedSubviews)
	{
		[[viewController view] removeFromSuperviewWithoutNeedingDisplay];
	}
	[displayedSubviews removeAllObjects];
	
    /* Call super-class reloadData */
    [super reloadData];
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
	/* Create paste-data property list */
	NSMutableArray *propertyList = [NSMutableArray array];
	
	/* Loop through each selected entity */
	NSMutableArray *graphEntityList = [NSMutableArray array];
	for (id item in items)
	{		
		/* Create entity descriptor */
		LCEntity *entity = [[item representedObject] object];
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
		
		/* Create dup properties dictionary */
		NSDictionary *properties = [NSDictionary dictionaryWithDictionary:[entityDesc properties]];
		
		/* Add properties to list */
		[propertyList addObject:properties];
		
		/* Get graphable entities */
		[graphEntityList addObjectsFromArray:[LCMetricGraphController graphableMetricsForEntities:[NSArray arrayWithObject:entity]]];
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

#pragma mark "Selection Highligh"

- (NSRect) highlightRectOfRow:(unsigned int)row
{
	NSRect rowRect = [self rectOfRow:row];
	
	if ([self levelForRow:row] > 0)
	{
		rowRect = NSMakeRect(NSMinX(rowRect), NSMinY(rowRect) - 17.0, NSWidth(rowRect), NSHeight(rowRect) + 17.0);
	}
	else if ([self isItemExpanded:[self itemAtRow:row]])
	{
		LCObjectTreeHeaderItem *headItem = [[self itemAtRow:row] representedObject];
		LCObjectTreeMetricItem *metricItem = [headItem.children objectAtIndex:0];
		rowRect = NSMakeRect(NSMinX(rowRect), NSMinY(rowRect) + 1.0, NSWidth(rowRect), NSHeight(rowRect) + metricItem.rowHeight + 1.0);		
	}
	
	return rowRect;
}



@end
