//
//  MBOidTextField.m
//  ModuleBuilder
//
//  Created by James Wilson on 15/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBOidTextField.h"

#import "MBContainer.h"

@implementation MBOidTextField

#pragma mark "Awake From NIB"

- (void) awakeFromNib 
{
	[self registerForDraggedTypes:[NSArray arrayWithObject:@"MBOid"]];
}

#pragma mark "Drag and Drop"

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)info 
{
	if ([self isEnabled])
	{
		[self setBackgroundColor:[NSColor colorWithDeviceRed:199.0/255.0 green:214.0/255.0 blue:242.0/255.0 alpha:1.0]];
		return NSDragOperationMove;
	}
	else
	{ return NSDragOperationNone; }
}

- (void)draggingExited:(id < NSDraggingInfo >)sender
{
	[self setBackgroundColor:[NSColor whiteColor]];
}

- (void)draggingEnded:(id < NSDraggingInfo >)sender
{
	[self setBackgroundColor:[NSColor whiteColor]];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)info
{
    NSPasteboard *pboard;
    pboard = [info draggingPasteboard];
	
	/* Get Entities */
	for (NSMutableData *data in [[info draggingPasteboard] propertyListForType:@"MBOid"])
	{
		MBOid *oid = [NSKeyedUnarchiver unarchiveObjectWithData:data];;
		[(MBContainer *)[viewController representedObject] setOid:oid];
	}	
	return YES;
}

@end
