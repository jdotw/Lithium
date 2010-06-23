//
//  MBEntity.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBEntity.h"
#import "MBContainer.h"
#import "ModuleDocument.h"
#import "MBDocumentWindowController.h"

@implementation MBEntity

#pragma mark "Constructors"

- (id) init
{
	[self setProperties:[NSMutableDictionary dictionary]];
	[self setChildren:[NSMutableArray array]];
	return self;
}

- (id) initWithCoder:(NSCoder *)decoder
{
	[self init];
	[self setProperties:[[decoder decodeObjectForKey:@"properties"] retain]];
	[self setChildren:[[decoder decodeObjectForKey:@"children"] retain]];
	NSEnumerator *childEnum = [[self children] objectEnumerator];
	MBEntity *child;
	while (child=[childEnum nextObject])
	{ [child setParent:self]; }
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
	[encoder encodeObject:children forKey:@"children"];
}

- (void) dealloc
{ 
	[properties release];
	[super dealloc];
}

- (MBEntity *) copy
{
	return [NSKeyedUnarchiver unarchiveObjectWithData:[NSKeyedArchiver archivedDataWithRootObject:self]];
}

#pragma mark "Properties" 

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{
	[properties release];
	properties = [dict retain];
}

#pragma mark View

- (NSView *) view
{
	return view;
}

#pragma mark "Children" 

- (NSMutableArray *) children
{ return children; }

- (void) setChildren:(NSMutableArray *)array
{ 
	[children release];
	children = [array retain];
}

- (void) insertObject:(MBEntity *)entity inChildrenAtIndex:(unsigned int)index
{ 
	[entity setDocument:[self document]];
	[[[document undoManager] prepareWithInvocationTarget:self] removeObjectFromChildrenAtIndex:index];
	[[self children] insertObject:entity atIndex:index]; 
	[entity setParent:self];
	if (![[document undoManager] isUndoing])
	{ [[document undoManager] setActionName:[NSString stringWithFormat:@"Add %@", [self typeString]]]; }
}

- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{
	MBEntity *entity = [[self children] objectAtIndex:index];
	[[[document undoManager] prepareWithInvocationTarget:self] insertObject:entity inChildrenAtIndex:index];
	[entity setParent:nil];
	[[self children] removeObjectAtIndex:index];
	if (![[document undoManager] isUndoing])
	{ [[document undoManager] setActionName:[NSString stringWithFormat:@"Remove %@", [self typeString]]]; }
}

- (void) moveChild:(MBEntity *)child toIndex:(unsigned int)index
{
	unsigned int currentIndex = [[self children] indexOfObject:child];
	if (currentIndex > index) currentIndex++;
	[[[document undoManager] prepareWithInvocationTarget:self] moveChild:child toIndex:currentIndex];
	[self willChangeValueForKey:@"children"];
	[[self children] insertObject:child atIndex:index];
	[[self children] removeObjectAtIndex:currentIndex];
	[self didChangeValueForKey:@"children"];
	if (![[document undoManager] isUndoing]) 
	{ [[document undoManager] setActionName:[NSString stringWithFormat:@"Move %@", [self typeString]]];	}
}

- (unsigned int) countOfChildren
{ 
	return [[self children] count];
}

- (MBEntity *) childNamed:(NSString *)desc
{
	for (MBEntity *child in [self children])
	{
		if ([[child desc] isEqualToString:desc]) return child;
	}
	return nil;
}

- (MBEntity *) container
{
	if ([[self type] intValue] == 4) return self;
	if ([[self type] intValue] == 6) return [self parent];
	if ([[self type] intValue] == 7) return [[self parent] parent];
	else return nil;
}

- (MBEntity *) metric
{
	if ([[self type] intValue] == 6) return self;
	if ([[self type] intValue] == 7) return [self parent];
	else return nil;	
}

- (MBEntity *) trigger
{
	if ([[self type] intValue] == 7) return self;
	else return nil;	
}


#pragma mark "View Controller"

- (NSViewController *) viewController
{
	return nil;
}

- (float) rowHeight
{ 
	return 68.0;
}

#pragma mark "Dependent Metric Binding"

- (void) bindDependents
{
	
}

#pragma mark "Duplicate Handling"

- (void) showDuplicateDescAlert
{
	/* Generic Handling */
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:[NSString stringWithFormat:@"Duplicate %@ Description", [self typeString]]];
	[alert setInformativeText:[NSString stringWithFormat:@"Each %@ must have a unique description.", [self typeString]]];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:[document windowForSheet] 
					  modalDelegate:self
					 didEndSelector:@selector(duplicateAlertDidEnd:returnCode:contextInfo:) 
						contextInfo:nil];
}

- (void) duplicateAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	[self setDesc:[self desc]];
}

#pragma mark "XML Output"

- (NSString *) xmlNodeName
{ return nil; }

- (NSString *) xmlChildrenNodeName
{ return nil; }

- (NSXMLNode *) xmlNode
{ return nil; }

#pragma mark "General Accessors"

- (id) document
{ return document; }

- (void) setDocument:(id)newDocument
{ 
	document = newDocument; 
	NSEnumerator *childEnum = [children objectEnumerator];
	MBEntity *child;
	while (child=[childEnum nextObject])
	{ [child setDocument:newDocument]; }
}

- (NSString *) desc
{ return [properties objectForKey:@"desc"]; }

- (void) setDesc:(NSString *)string
{ 
	/* Check for dup, if entity is part of the document */
	if (document)
	{
		NSArray *peers;
		if (parent) peers = [parent children];
		else peers = [(ModuleDocument *)document containers];
		for (MBEntity *peer in peers)
		{
			if (peer != self && [[peer desc] isEqualToString:string])
			{ 
				[self showDuplicateDescAlert];
				return;
			}
		}
	}
		
	/* Set Value */
	[[[document undoManager] prepareWithInvocationTarget:self] setDesc:[self desc]];
	if (string) [properties setObject:string forKey:@"desc"]; 
	else [properties removeObjectForKey:@"desc"];
	[[document undoManager] setActionName:@"Set Entity Description"];	
}

- (NSNumber *) type
{ return [properties objectForKey:@"type"]; }

- (void) setType:(NSNumber *)type
{ [properties setObject:type forKey:@"type"]; }

- (NSString *) typeString
{
	if ([[self type] intValue] == 4) return @"Container";
	if ([[self type] intValue] == 5) return @"Object";
	if ([[self type] intValue] == 6) return @"Metric";
	if ([[self type] intValue] == 7) return @"Trigger";
	else return @"Entity";
}

- (MBEntity *) parent
{ return parent; }

- (void) setParent:(MBEntity *)value
{ parent = value; }

- (BOOL) infoViewVisible
{ return [[properties objectForKey:@"infoViewVisible"] boolValue]; }

- (void) setInfoViewVisible:(BOOL)flag
{ 
	[[[document undoManager] prepareWithInvocationTarget:self] setInfoViewVisible:[self infoViewVisible]];
	[properties setObject:[NSNumber numberWithBool:flag] forKey:@"infoViewVisible"]; 
	[[document undoManager] setActionName:@"Show Entity Preferences"];
}

@end
