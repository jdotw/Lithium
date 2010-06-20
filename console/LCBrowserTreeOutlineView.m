//
//  LCBrowserTreeOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTreeOutlineView.h"

#import "LCBrowserTreeItem.h"
#import "LCEntity.h"
#import "LCSite.h"
#import "LCCustomer.h"
#import "LCBrowserTreeCell.h"
#import "LCBrowserTreeDevicesRoot.h"
#import "LCBrowserTreeXsanRoot.h"
#import "LCDeviceEditController.h"
#import "LCBrowserTreeDocumentsRoot.h"
#import "LCBrowserTreeScenes.h"
#import "LCBrowserTreeVRacks.h"
#import "LCBrowser2Controller.h"
#import "LCBrowserTreeGroupsCustomer.h"
#import "LCBrowserTreeApplicationsRoot.h"
#import "LCGroup.h"
#import "LCGroupEditWindowController.h"
#import "LCBrowserTreeCoreDeployment.h"

@interface LCBrowserTreeOutlineView (private)
- (NSMenu *) scenesMenuForEvent:(NSEvent *)event;
- (NSMenu *) vracksMenuForEvent:(NSEvent *)event;
- (NSMenu *) groupsMenuForEvent:(NSEvent *)event;
@end

@implementation LCBrowserTreeOutlineView

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	/* Setup Cells */
	entityCell = [[LCBrowserEntityCell alloc] init];
	textCell = [[LCTextFieldCell alloc] init];
	
	/* Set-up outline view */
	self.expansionPreferencePrefix = @"LCBrowserTreeOutlineView";
	[self registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", @"LCDeviceEditTemplate", @"LCGroup", @"LCBrowserTreeItemRoot", nil]];
	[self setDelegate:self];
	[self setDataSource:self];
	
	/* Listen for changes in item ordering */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(rootItemOrderChanged:)
												 name:@"LCBrowserTreeOutlineViewRootItemOrderChanged"
											   object:nil];
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:@"LCBrowserTreeOutlineViewRootItemOrderChanged"
												  object:nil];
	[super dealloc];
}

#pragma mark "Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(LCBrowserTreeCell *)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	cell.representedObject = [item representedObject];
	cell.parentRepresentedObject = [[self parentForItem:item] representedObject];
	cell.outlineViewEnabled = [self isEnabled];
	cell.selected = [self isRowSelected:[self rowForItem:item]];	
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	if ([self levelForRow:[self rowForItem:item]] == 0) 
	{
		/* Root Item */
		return 30.0;
	}
	else if ([[[item representedObject] class] isSubclassOfClass:[LCBrowserTreeItem class]])
	{ 
		/* Non-Root Item */
		return [(LCBrowserTreeItem *)[item representedObject] rowHeight];
	}
	else
	{ 
		/* Default (Non-Item) */
		return 15.0; 
	}
}

#pragma mark "Selection Management"

- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend
{
	LCBrowserTreeItem *treeItem = nil;
	if ([indexes firstIndex] != NSNotFound && [[[[self itemAtRow:[indexes firstIndex]] representedObject] class] isSubclassOfClass:[LCBrowserTreeItem class]]) 
	{ treeItem = [[self itemAtRow:[indexes firstIndex]] representedObject]; }
	if ([self levelForRow:[indexes firstIndex]] == 0 || (treeItem && ![treeItem selectable]))
	{ 
		/* Do not allow selection of root items */
		return;
	}
	
	/* Check if change in selection is permitted */
	id item = nil;
	if ([indexes firstIndex] != NSNotFound)
	{ item = [self itemAtRow:[indexes firstIndex]]; }		
	if (![(LCBrowser2Controller *)browser contentViewController] || [[(LCBrowser2Controller *)browser contentViewController] treeSelectionCanChangeToRepresent:[item representedObject]])
	{ 
		/* Allow change in selection */
		[super selectRowIndexes:indexes byExtendingSelection:extend]; 
	}
	else
	{ 
		/* Disallow change in selection */
		[super selectRowIndexes:[self selectedRowIndexes] byExtendingSelection:NO]; 
	}	
}

- (void) selectEntity:(LCEntity *)entity
{
	int i;
	
	/* Determine the actual entity to be selected */
	LCEntity *selectionCandidate = [entity device];
	
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		LCEntity *rowEntity = [item representedObject];
		
		if (![[rowEntity class] isSubclassOfClass:[LCEntity class]]) continue;
		
		if (rowEntity == selectionCandidate)
		{
			/* Match */
			if (i != [self selectedRow])
			{ 
				[self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO];
			}
			[self scrollRowToVisible:i];
			return;
		}
		else if ([selectionCandidate isDescendantOf:rowEntity])
		{
			/* Parent */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
	}
}

- (void) selectScenesRoot
{
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		if ([[item representedObject] isKindOfClass:[LCBrowserTreeDocumentsRoot class]])
		{ 
			/* Documents root */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([[item representedObject] isKindOfClass:[LCBrowserTreeScenes class]])
		{
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
		}
	}	
}

- (void) selectVRacksRoot
{
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		if ([[item representedObject] isKindOfClass:[LCBrowserTreeDocumentsRoot class]])
		{ 
			/* Documents root */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([[item representedObject] isKindOfClass:[LCBrowserTreeVRacks class]])
		{
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
		}
	}	
}

- (void) selectSceneDocument:(LCSSceneDocument *)document
{
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		if ([[item representedObject] isKindOfClass:[LCBrowserTreeDocumentsRoot class]])
		{ 
			/* Documents root */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([[item representedObject] isKindOfClass:[LCBrowserTreeScenes class]])
		{
			/* Scenes Tree */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([item representedObject] == document)
		{
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
		}
	}
}

- (void) selectVRackDocument:(LCVRackDocument *)document
{
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		if ([[item representedObject] isKindOfClass:[LCBrowserTreeDocumentsRoot class]])
		{ 
			/* Documents root */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([[item representedObject] isKindOfClass:[LCBrowserTreeVRacks class]])
		{
			/* Scenes Tree */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([item representedObject] == document)
		{
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
		}
	}
}

- (void) selectIncidentsItem
{
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{
		id item = [self itemAtRow:i];
		if ([[item representedObject] isKindOfClass:[LCBrowserTreeFaultsRoot class]])
		{ 
			/* Faults root */
			if (![self isItemExpanded:item])
			{ [self expandItem:item]; }
		}
		else if ([[item representedObject] isKindOfClass:[LCBrowserTreeIncidents class]])
		{
			if (i != [self selectedRow])
			{ [self selectRowIndexes:[NSIndexSet indexSetWithIndex:i] byExtendingSelection:NO]; }
		}
	}	
}

#pragma mark "Mouse Down Handling"

- (void) mouseDown:(NSEvent *)event
{
	if (![self isEnabled]) return;	
	[super mouseDown:event];
}

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:point];
	int col = [self columnAtPoint:point];
	
	if (row == -1 || col == -1) return nil;
	
	/* Check for root item */
	if ([[[[self itemAtRow:row] representedObject] class] isSubclassOfClass:[LCBrowserTreeItemRoot class]])
	{
		return rootItemsMenu;
	}

	if (![self isRowSelected:row])
	{ 
		[self selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
	}

	/* Find browserTree root item */
	id item = [self itemAtRow:row];
	id parent = [item parentNode];
	id root = parent;
	while ([[root parentNode] parentNode])
	{ root = [root parentNode]; }
	
	/* Return menu based on root type */
	if ([[root representedObject] class] == [LCBrowserTreeDevicesRoot class])
	{ 
		return [self devicesMenuForEvent:event]; 
	}
	else if ([[item representedObject] class] == [LCBrowserTreeScenes class] || [[item representedObject] class] == [LCSSceneDocument class])
	{
		return [self scenesMenuForEvent:event];
	}
	else if ([[item representedObject] class] == [LCBrowserTreeVRacks class] || [[item representedObject] class] == [LCVRackDocument class])
	{
		return [self vracksMenuForEvent:event];
	}
	else if ([[root representedObject] class] == [LCBrowserTreeGroupsRoot class])
	{
		return [self groupsMenuForEvent:event];
	}
	else if ([[item representedObject] class] == [LCBrowserTreeFaultsRoot class] || 
			 [[parent representedObject] class] == [LCBrowserTreeFaultsRoot class])
	{
		return faultsMenu;
	}
	else if ([[item representedObject] class] == [LCBrowserTreeApplicationsRoot class] || 
			 [[parent representedObject] class] == [LCBrowserTreeApplicationsRoot class])
	{
		return applicationsMenu;
	}
	else if ([[item representedObject] class] == [LCBrowserTreeCoreDeployment class])
	{
		[deploymentsMenuRefreshItem setRepresentedObject:[item representedObject]];
		return deploymentsMenu;
	}
	
	return [self menu];
}

- (NSMenu *) devicesMenuForEvent:(NSEvent *)event
{
	/* Selection-Specific Handling */
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:point];
	LCEntity *selectedEntity = [[self itemAtRow:row] representedObject];
	
	/* Set selective meny items */
	if ([[selectedEntity typeInteger] intValue] == 1)
	{
		/* Customer selected */
		[connectToMenu setEnabled:NO];
		[editSiteMenu setAction:nil];
		[removeSiteMenu setAction:nil];
		[addDeviceMenu setAction:nil];
		[editDeviceMenu setAction:nil];
		[removeDeviceMenu setAction:nil];
		[addServiceMenu setAction:nil];
		[manageServiceScriptsMenu setAction:nil];
	}
	else if ([[selectedEntity typeInteger] intValue] == 2)
	{
		/* Site selected */
		[connectToMenu setEnabled:NO];		
		[editSiteMenu setAction:@selector(editSiteClicked:)];
		[removeSiteMenu setAction:@selector(removeSiteClicked:)];
		[addDeviceMenu setAction:@selector(addNewDeviceClicked:)];
		[editDeviceMenu setAction:nil];
		[removeDeviceMenu setAction:nil];
		[addServiceMenu setAction:nil];
		[manageServiceScriptsMenu setAction:nil];
	}
	else if ([[selectedEntity typeInteger] intValue] == 3)
	{
		/* Device selected */
		[connectToMenu setEnabled:YES];
		[editSiteMenu setAction:nil];
		[removeSiteMenu setAction:nil];
		[addDeviceMenu setAction:@selector(addNewDeviceClicked:)];
		[editDeviceMenu setAction:@selector(editDeviceClicked:)];
		[removeDeviceMenu setAction:@selector(removeDeviceClicked:)];
		[addServiceMenu setAction:@selector(addNewServiceClicked:)];
		[manageServiceScriptsMenu setAction:@selector(manageServiceScriptsClicked:)];
	}
	
	return devicesMenu;
}

- (NSMenu *) scenesMenuForEvent:(NSEvent *)event
{
	/* Try to retrieve document */
//	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
//	int row = [self rowAtPoint:point];
//	LCSSceneDocument *document = nil;
//	if ([[[self itemAtRow:row] representedObject] class] == [LCSSceneDocument class])
//	{ document = [[self itemAtRow:row] representedObject]; }

	/* Clear old customer items */
	while ([[[scenesMenuAddItem submenu] itemArray] count] > 0) 
	{ [[scenesMenuAddItem submenu] removeItemAtIndex:0]; }
	
	/* Add customers */
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		/* Add item for each customer */
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:customer.displayString
										  action:@selector(addNewSceneToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:browser];
		[item setRepresentedObject:customer];
		[[scenesMenuAddItem submenu] insertItem:item atIndex:[[[scenesMenuAddItem submenu] itemArray] count]];			
		[item autorelease];
	}	
	
	return scenesMenu;
}

- (NSMenu *) vracksMenuForEvent:(NSEvent *)event
{
	/* Try to retrieve document */
//	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
//	int row = [self rowAtPoint:point];
//	LCVRackDocument *document = nil;
//	if ([[[self itemAtRow:row] representedObject] class] == [LCVRackDocument class])
//	{ document = [[self itemAtRow:row] representedObject]; }
	
	/* Clear old customer items */
	while ([[[vracksMenuAddItem submenu] itemArray] count] > 0) 
	{ [[vracksMenuAddItem submenu] removeItemAtIndex:0]; }
	
	/* Add customers */
	for (LCCustomer *customer in [LCCustomerList masterArray])
	{
		/* Add item for each customer */
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:customer.displayString
										  action:@selector(addNewVRackToCustomerMenuCicked:) 
								   keyEquivalent:@""];
		[item setTarget:browser];
		[item setRepresentedObject:customer];
		[[vracksMenuAddItem submenu] insertItem:item atIndex:[[[vracksMenuAddItem submenu] itemArray] count]];			
		[item autorelease];
	}
	
	return vracksMenu;
}

- (NSMenu *) groupsMenuForEvent:(NSEvent *)event
{
	/* Find group */
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:point];
	id item = [self itemAtRow:row];
	id parent = [self parentForItem:item];
	LCGroup *group = nil;
	if ([[[item representedObject] class] isSubclassOfClass:[LCEntity class]] && [[parent representedObject] class] == [LCGroup class])
	{ 
		/* Clicked on an entity that is directly below a group */
		group = [parent representedObject];
		[groupsMenuRemoveEntityItem setRepresentedObject:group];
		[groupsMenuRemoveEntityItem setHidden:NO];
	}
	else
	{
		[groupsMenuRemoveEntityItem setHidden:YES];
	}

	/* Reset entity-specific menus to hidden */
	[groupDeviceMenuItem setHidden:YES];
	[groupLocationMenuItem setHidden:YES];

	/* Enable entity-specific menu item */
	if ([[item representedObject] class] == [LCDevice class])
	{
		[groupDeviceMenuItem setHidden:NO];
	}
	else if ([[item representedObject] class] == [LCSite class])
	{
		[groupLocationMenuItem setHidden:NO];
	}
	
	return groupsMenu;
}

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)outlineView:(NSOutlineView *)outlineView 
				  validateDrop:(id <NSDraggingInfo>)info 
				  proposedItem:(id)item 
			proposedChildIndex:(int)index
{
	/* Check for a device move */
	if ([[info draggingSource] class] == [self class] && 
		[[item representedObject] class] == [LCSite class] && 
		[[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"])
	{
		/* Source is a BrowserTreeOutlineView and the destination is a site. 
		 * Now validate the drop entities 
		 */		
		LCSite *site = [item representedObject];
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		for (NSDictionary *properties in propertiesArray)
		{
			/* Check to see that all entities are devices,
			 * belong to the same customer as the site, 
			 * and aren't from the destination site
			 */
			LCEntity *entity = [[LCEntityDescriptor descriptorWithProperties:properties] locateEntity:YES];
			if (entity.type != 3 || entity.customer != site.customer || entity.site == site)
			{ return NSDragOperationNone; }
		}
		
		return NSDragOperationMove;
	}
	
	/* Check for device template drop */
	if ([[item representedObject] class] == [LCSite class] && 
		[[info draggingPasteboard] propertyListForType:@"LCDeviceEditTemplate"])
	{
		/* Destination is a site, and the drop data contains LCDeviceEditTemplates */	
		return NSDragOperationCopy;
	}
	
	/* Check for group drop target (entity drop) */
	if ([[item representedObject] class] == [LCGroup class] && 
		[[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"])
	{
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		for (NSDictionary *properties in propertiesArray)
		{
			/* Check to see that all entities are devices,
			 * belong to the same customer as the site, 
			 * and aren't from the destination site
			 */
			LCGroup *group = [item representedObject];
			LCEntity *entity = [[LCEntityDescriptor descriptorWithProperties:properties] locateEntity:YES];
			if (entity.customer == group.customer)
			{ 
				if ([properties objectForKey:@"group_id"])
				{
					int currentGroupID = [[properties objectForKey:@"group_id"] intValue];
					if (currentGroupID != group.groupID && ![group.children containsObject:entity])
					{ return NSDragOperationMove; }
				}
				else if (![group.children containsObject:entity])
				{ return NSDragOperationCopy; }
				else
				{ return NSDragOperationNone; }
			}
		}
	}	
	
	/* Check for group drop target (group drop) */
	if (([[item representedObject] class] == [LCGroup class] || [[item representedObject] class] == [LCBrowserTreeGroupsCustomer class]) && 
		[[info draggingPasteboard] propertyListForType:@"LCGroup"])
	{
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCGroup"];
		for (NSDictionary *properties in propertiesArray)
		{
			LCGroup *destinationGroup = nil;
			if ([[item representedObject] class] == [LCGroup class]) destinationGroup = [item representedObject];
			
			LCGroup *group = [destinationGroup.customer.groupTree.groupDictionary objectForKey:[properties objectForKey:@"group_id"]];
			if (group && group != destinationGroup) return NSDragOperationMove;
		}
	}	
	
	if ([info draggingSource] == self && [item representedObject] == nil &&
		[[info draggingPasteboard] propertyListForType:@"LCBrowserTreeItemRoot"])
	{
		/* Source is this outline view, dropping on root with rootitem on pasteboard */
		return NSDragOperationMove;
	}
	
	
	/* Default to disallow */
	return NSDragOperationNone;
}


- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 acceptDrop:(id <NSDraggingInfo>)info 
			   item:(id)item 
		 childIndex:(int)index
{
	/* Check for device move */
	if ([[info draggingSource] class] == [self class] && [[item representedObject] class] == [LCSite class])
	{
		/* Perform move */
		NSMutableArray *devices = [NSMutableArray array];
		LCSite *site = [item representedObject];
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		for (NSDictionary *properties in propertiesArray)
		{ [devices addObject:[[LCEntityDescriptor descriptorWithProperties:properties] locateEntity:YES]]; }
		[[LCDeviceEditController alloc] initWithDevicesToMove:devices 
													   toSite:site 
											   windowForSheet:[self window]];
		
		return YES;
		
	}
	
	/* Check for template drop (device add) */
	if ([[item representedObject] class] == [LCSite class] && 
		[[info draggingPasteboard] propertyListForType:@"LCDeviceEditTemplate"])
	{
		/* Destination is a site, and the drop data contains LCDeviceEditTemplates */
		NSMutableArray *templates = [NSMutableArray array];
		LCSite *site = [item representedObject];
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCDeviceEditTemplate"];
		for (NSDictionary *properties in propertiesArray)
		{ [templates addObject:[LCDeviceEditTemplate templateWithProperties:properties]]; }		
		LCDeviceEditController *editController = [[LCDeviceEditController alloc] initForNewDevicesAtSite:site usingTemplates:templates];
		[NSApp beginSheet:[editController window]
		   modalForWindow:[self window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
		
		return YES;
	}	
	
	/* Check for group drop target (entity drop) */
	if ([[item representedObject] class] == [LCGroup class] && 
		[[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"])
	{
		/* Get Entities */
		LCGroup *group = [item representedObject];
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
		for (NSDictionary *properties in propertiesArray)
		{ 
			LCEntity *entity = [[LCEntityDescriptor descriptorWithProperties:properties] locateEntity:YES];
			if ([properties objectForKey:@"group_id"])
			{
				LCGroup *previousParent = [group.customer.groupTree.groupDictionary objectForKey:[properties objectForKey:@"group_id"]];
				[[LCGroupEditWindowController alloc] initWithEntity:entity
													toMoveFromGroup:previousParent
															toGroup:group 
													 windowForSheet:[self window]];
			}
			else
			{ 
				[[LCGroupEditWindowController alloc] initWithEntity:entity toAddToGroup:group windowForSheet:[self window]]; 
			}
		}
		return YES;
	}
	
	/* Check for group drop target (group drop) */
	if (([[item representedObject] class] == [LCGroup class] || [[item representedObject] class] == [LCBrowserTreeGroupsCustomer class]) && 
		[[info draggingPasteboard] propertyListForType:@"LCGroup"])
	{
		NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCGroup"];
		for (NSDictionary *properties in propertiesArray)
		{
			LCGroup *destinationGroup = nil;
			if ([[item representedObject] class] == [LCGroup class]) destinationGroup = [item representedObject];
			
			LCGroup *group = [destinationGroup.customer.groupTree.groupDictionary objectForKey:[properties objectForKey:@"group_id"]];
			
			if (group)
			{
				[[LCGroupEditWindowController alloc] initWithGroupToMove:group 
																toParent:destinationGroup
														  windowForSheet:[self window]];
			}
		}
		
		return YES;
	}	
	
	/* Check for root item re-arrange */
	if ([info draggingSource] == self && [item representedObject] == nil &&
		[[info draggingPasteboard] propertyListForType:@"LCBrowserTreeItemRoot"])
	{
		/* Source is this outline view, dropping on root with rootitem on pasteboard */
		LCBrowser2Controller *browserController = (LCBrowser2Controller *) browser;
		for (NSDictionary *properties in [[info draggingPasteboard] propertyListForType:@"LCBrowserTreeItemRoot"])
		{
			NSString *className = [properties objectForKey:@"class"];
			LCBrowserTreeItemRoot *sourceRootItem = nil; 
			for (LCBrowserTreeItemRoot *rootItem in [browserController.browserTree items])
			{
				if ([[rootItem className] isEqualToString:className]) 
				{ 
					sourceRootItem = rootItem;
					break;
				}					
			}
			if (sourceRootItem)
			{			
				/* Get index */
				int oldIndex = [browserController.browserTree.items indexOfObject:sourceRootItem];
				int newIndex = index;
				if (oldIndex < newIndex) newIndex--;
				
				/* Move item in array */
				[browserController.browserTree removeObjectFromItemsAtIndex:oldIndex];
				[browserController.browserTree insertObject:sourceRootItem inItemsAtIndex:newIndex];
				
				/* Update preferred indexes */
				for (LCBrowserTreeItemRoot *rootItem in [browserController.browserTree items])
				{
					rootItem.preferredIndex = [browserController.browserTree.items indexOfObject:rootItem];
				}
				
				/* Expand */
				[self expandAllItemsUsingPreferences];
				
				/* Notify other windows */
				[[NSNotificationCenter defaultCenter] postNotificationName:@"LCBrowserTreeOutlineViewRootItemOrderChanged"
																	object:self];
			}
		}

		return NSDragOperationMove;
	}
	
	

	return NO;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView 
		 writeItems:(NSArray *)items 
	   toPasteboard:(NSPasteboard *)pboard
{
	/* Create paste-data property list */
	NSMutableArray *entityPropertyList = [NSMutableArray array];
	NSMutableArray *groupPropertyList = [NSMutableArray array];
	NSMutableArray *rootItemPropertyList = [NSMutableArray array];
	
	/* Loop through each selected item */
	for (id item in items)
	{	
		/* Get representedObject */
		if ([[[item representedObject] class] isSubclassOfClass:[LCEntity class]])
		{		
			/* Create entity descriptor */
			LCEntity *entity = (LCEntity *) [item representedObject];
			LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorForEntity:entity];
			
			/* Create dup properties dictionary */
			NSMutableDictionary *properties = [NSMutableDictionary dictionaryWithDictionary:[entityDesc properties]];
			
			/* Check for group */
			id parentItem = [self parentForItem:item];
			if ([[parentItem representedObject] class] == [LCBrowserTreeGroupsCustomer class])
			{ [properties setObject:@"0" forKey:@"group_id"]; }
			else if ([[parentItem representedObject] class] == [LCGroup class])
			{ 
				LCGroup *parentGroup = [parentItem representedObject];
				[properties setObject:[NSString stringWithFormat:@"%i", parentGroup.groupID] forKey:@"group_id"]; 
			}
			
			/* Add properties to list */
			[entityPropertyList addObject:properties];
		}
		else if ([[item representedObject] class] == [LCGroup class])
		{
			LCGroup *group = [item representedObject];
			NSMutableDictionary *properties = [NSMutableDictionary dictionary];
			[properties setObject:[NSString stringWithFormat:@"%i", group.groupID] forKey:@"group_id"];
			[properties setObject:group.customer.name forKey:@"cust_name"];
			[groupPropertyList addObject:properties];
		}
		else if ([[[item representedObject] class] isSubclassOfClass:[LCBrowserTreeItemRoot class]])
		{
			LCBrowserTreeItemRoot *rootItem = (LCBrowserTreeItemRoot *) [item representedObject];
			NSMutableDictionary *properties = [NSMutableDictionary dictionary];
			[properties setObject:[rootItem className] forKey:@"class"];			
			[rootItemPropertyList addObject:properties];
		}
	}
	
	NSMutableArray *types = [NSMutableArray array];
	if (entityPropertyList.count > 0) [types addObject:@"LCEntityDescriptor"];
	if (groupPropertyList.count > 0) [types addObject:@"LCGroup"];
	if (rootItemPropertyList.count > 0) [types addObject:@"LCBrowserTreeItemRoot"];

	[pboard declareTypes:types owner:self];
	
	if (entityPropertyList.count > 0) [pboard setPropertyList:entityPropertyList forType:@"LCEntityDescriptor"];
	if (groupPropertyList.count > 0) [pboard setPropertyList:groupPropertyList forType:@"LCGroup"];
	if (rootItemPropertyList.count > 0) [pboard setPropertyList:rootItemPropertyList forType:@"LCBrowserTreeItemRoot"];
	
	if (types.count > 0) return YES;
	else return NO;
}

- (void) rootItemOrderChanged:(NSNotification *)note
{
	if ([note object] != self)
	{
		/* Other LCBrowserTreeOutlineView has re-ordered root items */
		LCBrowser2Controller *browserController = browser;
		[browserController.browserTree resetRootItems];
	}
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


@synthesize entityCell;
@synthesize textCell;
@synthesize devicesMenu;
@synthesize servicesMenu;
@synthesize connectToMenu;
@synthesize addSiteMenu;
@synthesize editSiteMenu;
@synthesize removeSiteMenu;
@synthesize addDeviceMenu;
@synthesize editDeviceMenu;
@synthesize removeDeviceMenu;
@synthesize addServiceMenu;
@synthesize manageServiceScriptsMenu;
@synthesize browser;
		 
@end
