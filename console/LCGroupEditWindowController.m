//
//  LCGroupEditWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 10/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGroupEditWindowController.h"

@interface LCGroupEditWindowController (private)
- (void) performXmlAction:(NSString *)xmlName;
- (void) updateLiveGroup;
@end

@implementation LCGroupEditWindowController

#pragma mark "Constructors"

- (id) initForNewGroupUnderParent:(LCGroup *)initParent customer:(LCCustomer *)initCustomer windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	[self loadWindow];
	
	self.customer = initCustomer;
	self.parent = initParent;
	self.windowForSheet = initWindow;
	self.group = [[LCGroup new] autorelease];
	self.group.customer = self.customer;
	self.group.parent = self.parent;
	self.group.parentID = self.parent.groupID;
	addGroup = YES;
	
	[titleTextField setStringValue:@"Create Group"];
	[okButton setTitle:@"Create"];

	[NSApp beginSheet:[self window]
	   modalForWindow:self.windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	sheetShown = YES;
	
	return self;
}

- (id) initWithGroupToEdit:(LCGroup *)initGroup customer:(LCCustomer *)initCustomer windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initCustomer;
	self.group = initGroup;
	self.parent = self.group.parent;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;
	[self loadWindow];
	
	[titleTextField setStringValue:@"Rename Group"];
	[okButton setTitle:@"Save"];
	
	[NSApp beginSheet:[self window]
	   modalForWindow:self.windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	sheetShown = YES;
	
	return self;
}

- (id) initWithGroupToMove:(LCGroup *)initGroup toParent:(LCGroup *)initParent windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.group = initGroup;
	self.customer = self.group.customer;
	self.previousParent = self.group.parent;
	self.parent = initParent;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;

	
	[self performXmlAction:@"group_update"];
	
	/* Move group */
	if (previousParent)
	{ [previousParent removeObjectFromChildrenAtIndex:[previousParent.children indexOfObject:group]]; }
	else
	{ [customer.groupTree removeObjectFromGroupsAtIndex:[customer.groupTree.groups indexOfObject:group]]; }
	if (self.parent)
	{ [parent insertObject:self.group inChildrenAtIndex:parent.children.count]; }
	else
	{ [customer.groupTree insertObject:group inGroupsAtIndex:customer.groupTree.groups.count]; }
	self.group.parent = self.parent;
	
	return self;	
}

- (id) initWithGroupToDelete:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initGroup.customer;
	self.group = initGroup;
	self.parent = self.group.parent;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;
	removeGroup = YES;

	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Group Delete"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert beginSheetModalForWindow:self.windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(deleteGroupAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
	
	return self;	
}

- (id) initWithEntity:(LCEntity *)initEntity toAddToGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initGroup.customer;
	self.group = initGroup;
	self.parent = self.group;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;
	self.entity = initEntity;

	[self performXmlAction:@"group_entity_add"];
	
	[self.parent.childrenDictionary setObject:entity forKey:[[entity entityDescriptor] addressString]];
	[self.parent insertObject:entity inChildrenAtIndex:self.parent.children.count];
	
	return self;	
}

- (id) initWithEntity:(LCEntity *)initEntity toMoveFromGroup:(LCGroup *)initPreviousGroup toGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initGroup.customer;
	self.group = initGroup;
	self.parent = initGroup;
	self.previousParent = initPreviousGroup;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;
	self.entity = initEntity;
	
	[self performXmlAction:@"group_entity_move"];
	
	[self.previousParent.childrenDictionary removeObjectForKey:[[entity entityDescriptor] addressString]];
	[self.previousParent removeObjectFromChildrenAtIndex:[self.previousParent.children indexOfObject:entity]];
	[self.parent.childrenDictionary setObject:entity forKey:[[entity entityDescriptor] addressString]];
	[self.parent insertObject:entity inChildrenAtIndex:self.parent.children.count];
	
	return self;	
}

- (id) initWithEntity:(LCEntity *)initEntity toRemoveFromGroup:(LCGroup *)initGroup windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"GroupEditWindow" owner:self];
	if (!self) return nil;
	
	self.customer = initGroup.customer;
	self.group = initGroup;
	self.parent = self.group;
	self.desc = self.group.desc;
	self.windowForSheet = initWindow;
	self.entity = initEntity;
	
	[self performXmlAction:@"group_entity_remove"];
	
	[self.parent.childrenDictionary removeObjectForKey:[[entity entityDescriptor] addressString]];
	[self.parent removeObjectFromChildrenAtIndex:[self.parent.children indexOfObject:entity]];
	
	return self;		
}

- (void) dealloc
{
	[desc release];
	[group release];
	[parent release];
	[customer release];
	[windowForSheet release];
	[previousParent release];
	[entity release];
	[super dealloc];
}

#pragma mark "UI Actions"

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
	[self autorelease];
}

- (IBAction) saveClicked:(id)sender
{
	[self updateLiveGroup];
	if (group.groupID == 0)
	{ [self performXmlAction:@"group_create"]; }
	else
	{ [self performXmlAction:@"group_update"]; }
}

- (void) deleteGroupAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Perform delete */
		[self performXmlAction:@"group_delete"];
	}
}

#pragma mark "XML Methods"

- (void) updateLiveGroup
{
	/* Updates the live LCGroup based on the user input */
	if (self.desc && [self.desc length] > 0)
	{ self.group.desc = self.desc; }
	else
	{ self.group.desc = @"Untitled Group"; }
}

- (void) performXmlAction:(NSString *)xmlName
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"group"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"desc" 
									  stringValue:self.group.desc]];
	if (self.group.groupID > 0)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"id" 
										  stringValue:[NSString stringWithFormat:@"%i", self.group.groupID]]];
	}
	if (self.parent.groupID > 0)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"parent" 
										  stringValue:[NSString stringWithFormat:@"%i", self.parent.groupID]]];
	}
	if (self.previousParent.groupID > 0)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"prev_parent" 
										  stringValue:[NSString stringWithFormat:@"%i", self.previousParent.groupID]]];
	}
	if (self.entity)
	{
		[rootnode addChild:[[self.entity entityDescriptor] xmlNode]];
	}
	
	
	/* Create and perform request */
	xmlRequest = [[LCXMLRequest requestWithCriteria:customer
										   resource:[customer resourceAddress]
											 entity:[customer entityAddress]
											xmlname:xmlName
											 refsec:0
											 xmlout:xmldoc] retain];
	[xmlRequest setDelegate:self];
	[xmlRequest setThreadedXmlDelegate:self];
	[xmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[xmlRequest performAsyncRequest];
	
	/* Set Status */
	self.xmlOperationInProgress = YES;		
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Check for Error */
	if ([rootNode.properties objectForKey:@"error"])
	{
		/* Error Occurred */
		self.status = [rootNode.properties objectForKey:@"error"];
		shouldClose = NO;
	}
	else if ([rootNode.properties objectForKey:@"id"])
	{
		/* ID Received */
		self.group.groupID = [[rootNode.properties objectForKey:@"id"] intValue];
		shouldClose = YES;
		if (addGroup)
		{
			[customer.groupTree.groupDictionary setObject:self.group forKey:[rootNode.properties objectForKey:@"id"]];
			if (self.parent)
			{ [parent insertObject:self.group inChildrenAtIndex:parent.children.count]; }
			else
			{ [customer.groupTree insertObject:group inGroupsAtIndex:customer.groupTree.groups.count]; }
		}
	}
	else if ([rootNode.properties objectForKey:@"result"])
	{
		shouldClose = YES;
	}
	else
	{
		/* Neither an error nor an ID received */
		self.status = @"ERROR: Server did not return a response.";
		shouldClose = NO;
	}
	
	if (removeGroup)
	{
		[customer.groupTree.groupDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", group.groupID]];
		if (parent)
		{ [parent removeObjectFromChildrenAtIndex:[parent.children indexOfObject:group]]; }
		else 
		{ [customer.groupTree removeObjectFromGroupsAtIndex:[customer.groupTree.groups indexOfObject:group]];	}
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Update status */
	self.xmlOperationInProgress = NO;
	if (![sender success])
	{ self.status = @"Failed to send request to Lithium Core"; }
	
	/* Close window if finished */
	if (shouldClose)
	{
		if (sheetShown)	
		{
			[NSApp endSheet:[self window]];
			[[self window] close];
		}
		[self autorelease];
	}
	
	/* Cleanup */
	[xmlRequest release];
	xmlRequest = nil;
}

#pragma mark "Properties"

@synthesize desc;
@synthesize group;
@synthesize parent;
@synthesize previousParent;
@synthesize customer;
@synthesize xmlOperationInProgress;
@synthesize status;
@synthesize windowForSheet;
@synthesize entity;

@end
