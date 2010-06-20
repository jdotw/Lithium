//
//  LCCaseEntityArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 7/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCaseEntityArrayController.h"


@implementation LCCaseEntityArrayController

#pragma mark Drag and Drop Methods

- (NSDragOperation)tableView:(NSTableView*)tv
				validateDrop:(id <NSDraggingInfo>)info
				 proposedRow:(int)row
	   proposedDropOperation:(NSTableViewDropOperation)dragOp
{
	/* Retrieve Entity Descriptors from pasteboard */
	NSArray *entityDescriptorArray = [[info draggingPasteboard] propertyListForType:@"LCEntityDescriptor"];

	/* Validate entities against existing case */
	if (cas && [cas customer] && [[cas customer] name])
	{
		/* Loop through dropping entities */
		NSDictionary *entityDescriptorProperties;
		for (entityDescriptorProperties in entityDescriptorArray)
		{
			LCEntityDescriptor *entityDescriptor = [LCEntityDescriptor descriptorWithProperties:entityDescriptorProperties];
			
			/* Check customer matches case */
			if ([[[cas customer] name] isEqualToString:[entityDescriptor cust_name]] == NO)
			{ 
				/* Does not match, entity is from another customer.
				 * Deny the drop action
				 */
				return NSDragOperationNone; 
			}
		}
	}
	
	/* Call super-class method */
	dragOp = [super tableView:tv validateDrop:info proposedRow:row proposedDropOperation:dragOp];
	
	return dragOp;
}

#pragma mark "Properties"
@synthesize cas;

@end
