//
//  LCIPRegistryTreeController.m
//  Lithium Console
//
//  Created by James Wilson on 19/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCIPRegistryTreeController.h"
#import "LCIPRegistryEntry.h"

@implementation LCIPRegistryTreeController

#pragma mark "NIB Methods"

- (void)awakeFromNib
{
	[outlineView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCDeviceEditTemplate", nil]];
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
	/* Loop through each item to create properties */
	NSMutableArray *propertyList = [NSMutableArray array];
	for (id item in items)
	{
		/* Check the item is a service */
		if (![[[item representedObject] class] isSubclassOfClass:[LCIPRegistryEntry class]])
		{ continue; }
		
		/* Add service properties to list */
		[propertyList addObject:[[(LCIPRegistryEntry *)[item representedObject] deviceTemplate] properties]];
	}
	
	if (propertyList.count > 0) 
	{
		[pboard declareTypes:[NSArray arrayWithObject:@"LCDeviceEditTemplate"] owner:self];
		[pboard setPropertyList:propertyList forType:@"LCDeviceEditTemplate"];
		return YES;
	}
	else return NO;
}

#pragma mark NSOutlineView Hacks for Pretending to be a data source

- (BOOL) outlineView: (NSOutlineView *)ov isItemExpandable:(id)item 
{ return NO; }

- (int) outlineView: (NSOutlineView *)ov numberOfChildrenOfItem:(id)item 
{ return 0; }

- (id)   outlineView: (NSOutlineView *)ov child:(int)index ofItem:(id)item 
{ return nil; }

- (id)   outlineView: (NSOutlineView *)ov objectValueForTableColumn:(NSTableColumn*)col byItem:(id)item 
{ return nil; }


@end
