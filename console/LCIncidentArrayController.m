//
//  LCIncidentArrayController.m
//  Lithium Console
//
//  Created by James Wilson on 3/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCIncidentArrayController.h"
#import "LCAction.h"
#import "LCIncident.h"
#import "LCCustomer.h"

@implementation LCIncidentArrayController

#pragma mark Drag and Drop Methods

- (BOOL)tableView:(NSTableView *)tv
		writeRows:(NSArray*)rows
	 toPasteboard:(NSPasteboard*)pboard
{
	NSMutableArray *propertyList = [NSMutableArray array];
	
	/* Declare types */
	[pboard declareTypes:[NSArray arrayWithObject:@"LCEntityDescriptor"] owner:self];
	
	/* Create/Paste property list */
	NSNumber *index;
	for (index in rows)
	{
		NSMutableDictionary *properties;
		
		/* Create dup properties dictionary */
		properties = [NSMutableDictionary dictionaryWithDictionary:[[[self arrangedObjects] objectAtIndex:[index intValue]] properties]];
		
		/* Add properties to list */
		[propertyList addObject:properties];
	}
	[pboard setPropertyList:propertyList forType:@"LCEntityDescriptor"];

	return YES;
}

#pragma mark Menu Items

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	/* Get selected */
	if ([[self selectedObjects] count] < 1) return nil;
	LCIncident *selectedIncident = [[self selectedObjects] objectAtIndex:0];
	
	/* Loop through actions */
	NSEnumerator *actionEnum = [[selectedIncident actions] objectEnumerator];
	LCAction *action;
	NSMenuItem *actionsItem = [menuTemplate itemWithTitle:@"Execute Action"];
	while ([[[actionsItem submenu] itemArray] count] > 0)
	{ [[actionsItem submenu] removeItemAtIndex:0]; }
	while (action=[actionEnum nextObject])
	{
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:[action desc]
													  action:@selector(execute)
											   keyEquivalent:@""];
		if ([(LCCustomer *)[[selectedIncident entity] customer] userIsNormal]) [item setTarget:action];
		[[actionsItem submenu] insertItem:item atIndex:[[[actionsItem submenu] itemArray] count]];
		[item autorelease];
	}
	
	
	return menuTemplate;
}

@end
