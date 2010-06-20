//
//  LCMultiGraphDeviceArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 14/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCMultiGraphDeviceArrayController.h"
#import "LCEntityDescriptor.h"

@implementation LCMultiGraphDeviceArrayController

#pragma mark "Drag and Drop Methods"

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	
	/* Process each descriptor */
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		/* Create descriptor and locate entity */
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
		if ([[entityDesc type_num] intValue] > 4) 
		{
			/* Device is not customer, site or device */
			return NO;
		}
	}
	
	/* Call super-class method */
	dragOp = [super tableView:tv validateDrop:info proposedRow:row proposedDropOperation:dragOp];
	
	return dragOp;
}

- (BOOL)tableView:(NSTableView*)tv
	   acceptDrop:(id <NSDraggingInfo>)info
			  row:(int)row
	dropOperation:(NSTableViewDropOperation)op
{
	/* Adds the dragged entity to the list.
	 * If a customer or site is dropped, all devices
	 * present at that customer/site are added
	 */
	
	/* Get Entity Descriptors */
	NSArray *propertiesArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];
	
	/* Process each descriptor */
	NSDictionary *properties;
	for (properties in propertiesArray)
	{
		/* Create descriptor and locate entity */
		LCEntityDescriptor *entityDesc = [LCEntityDescriptor descriptorWithProperties:properties];
		LCEntity *entity = [entityDesc locateEntity:YES];

		NSArray *devices = nil;
		switch ([[entity typeInteger] intValue])
		{
			case 1:		/* Customer dropped */
				devices = [entity valueForKeyPath:@"children.@unionOfArrays.children"];
				break;
			case 2:		/* Site dropped */
				devices = [entity valueForKeyPath:@"children"];
				break;
			case 3:		/* Device dropped */
				devices = [NSArray arrayWithObject:entity];
				break;
		}
		
		/* Add entities to list */
		if (devices) [self addObjects:devices];
	}
	
	return YES;
}

@end
