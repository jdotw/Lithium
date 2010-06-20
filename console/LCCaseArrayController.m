//
//  LCCaseArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 7/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseArrayController.h"

#import "LCCase.h"
#import "LCEntity.h"
#import "LCEntityDescriptor.h"

@implementation LCCaseArrayController

#pragma mark NIB Methods

- (void)awakeFromNib
{
	[self setAllowDrop:YES];
    [tableView registerForDraggedTypes:[NSArray arrayWithObjects:@"LCEntityDescriptor", nil]];
	[super awakeFromNib];
}

#pragma mark Drag and Drop Methods

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	/* Entities (and entities from incidents) can be dropped on
	 * to cases. This function checks the 
	 * operation is valid and allows the drop
	 */
	
	/* Check config */
	if (allowDrop == NO)
	{ return NSDragOperationNone; }
	
	/* Check for a move operations */
	if ([info draggingSource] == tableView)
	{
		/* Do not allow move */
		return NSDragOperationNone; 
	}
	
	/* Check row is in range */
	if (row >= [[self arrangedObjects] count])
	{ 
		/* No case at row */
		return NSDragOperationNone; 
	}
	
	/* Check entities belong to cases customer */
	LCCase *destCase = [[self arrangedObjects] objectAtIndex:row];
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:properties];
		if ([[entityDescriptor cust_name] isEqualToString:[[destCase customer] name]] == NO)
		{ 
			/* Entity belongs to a foreign customer. Deny the drop */
			return NSDragOperationNone;
		}
	}
		
	/* Config drop location */
	if (row >= 0)
	{ 
		[tv setDropRow:row dropOperation:NSTableViewDropOn]; 
	}
	else
	{ 
		/* No row to drop to, deny */
		return NSDragOperationNone;
	}
	
    return NSDragOperationCopy;
}

- (BOOL)tableView:(NSTableView*)tv
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row
	dropOperation:(NSTableViewDropOperation)op
{
	/* Find case */
	LCCase *destCase = [[self arrangedObjects] objectAtIndex:row];
	
	/* Add pasted rows */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:properties];
		LCEntity *entity = [entityDescriptor locateEntity:YES];
		[[destCase entityList] insertObject:entity inEntitiesAtIndex:0];
	}
	
	return YES;
}

#pragma mark "Accessor Methods"
@synthesize allowDrop;

@end
