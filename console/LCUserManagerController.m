//
//  LCUserManagerController.m
//  Lithium Console
//
//  Created by James Wilson on 21/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCUserManagerController.h"


@implementation LCUserManagerController

#pragma mark "Add/Edit User"

- (IBAction) addUserClicked:(id)sender
{
	/* Create dummy user */
	[self setEditUser:[[LCUser new] autorelease]];
	
	/* Set defaults */
	[[editUser properties] setObject:@"30" forKey:@"level_num"];
	
	/* Open Sheet */
	[self setResultString:@""];
	[self setResultIcon:nil];
	[saveButton setTitle:@"Add"];
	[NSApp beginSheet:addUserSheet 
	   modalForWindow:[setupController window]
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];		
	[usernameField setEnabled:YES];
	[addUserSheet makeFirstResponder:usernameField];
}

- (IBAction) addUserSheetCancelClicked:(id)sender
{
	[NSApp endSheet:addUserSheet];
	[addUserSheet close];
	[self setEditUser:nil];
}

- (IBAction) editUserClicked:(id)sender
{
	/* Get Selection */
	if ([[userArrayController selectedObjects] count] < 1) return;
	[self setEditUser:[LCUser userWithProperties:[(LCUser *)[[userArrayController selectedObjects] objectAtIndex:0] properties]]];
	
	/* Check for global admin */
	if ([editUser isGlobalAdmin])
	{
		[self setEditUser:nil];
		[NSApp beginSheet:editGlobalAdminSheet
		   modalForWindow:[setupController window]
			modalDelegate:self 
		   didEndSelector:nil
			  contextInfo:nil];
		return;
	}
		
	/* Open Sheet */
	[self setResultString:@""];
	[self setResultIcon:nil];
	[saveButton setTitle:@"Save"];
	[NSApp beginSheet:addUserSheet 
	   modalForWindow:[setupController window]
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];
	[usernameField setEnabled:NO];
	[addUserSheet makeFirstResponder:passwordField];
}

- (IBAction) editGlobalAdminCancelClicked:(id)sender
{
	[NSApp endSheet:editGlobalAdminSheet];
	[editGlobalAdminSheet close];
}

- (IBAction) saveUserSheetAddClicked:(id)sender
{
	/* Add/Update user to Core */
	
	/* Check passwords */
	if (![[[editUser properties] objectForKey:@"password"] isEqualToString:[[editUser properties] objectForKey:@"confirm_password"]])
	{
		[self setResultString:@"Passwords do not match, try again."];
		[self setResultIcon:[NSImage imageNamed:@"stop_16.tif"]];
		return;
	}
	
	/* Check State */
	if (xmlOperationInProgress) return;
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"user"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"username" stringValue:[editUser username]]];
	NSEnumerator *keyEnum = [[editUser properties] keyEnumerator];
	NSString *key;
	while (key = [keyEnum nextObject])
	{
		if (![key isEqualToString:@"username"])
		{
			id value = [[editUser properties] objectForKey:key];
			[rootnode addChild:[NSXMLNode elementWithName:key stringValue:[value description]]];
		}
	}
	
	/* Create and perform request */
	updateXmlReq = [[LCXMLRequest requestWithCriteria:[setupController customer]
									   resource:[[setupController customer] resourceAddress]
										 entity:[[setupController customer] entityAddress]
										xmlname:@"user_update"
										 refsec:0
										 xmlout:xmldoc] retain];
	[updateXmlReq setDelegate:self];
	[updateXmlReq setPriority:XMLREQ_PRIO_HIGH];
	[updateXmlReq performAsyncRequest];	
	[self setXmlOperationInProgress:YES];
	[self setResultIcon:nil];
	[self setResultString:@"Saving user info..."];	
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Set flag */
	[self setXmlOperationInProgress:NO];
	
	/* Release request */
	if (sender == updateXmlReq) 
	{
		/* Add/Edit Finished */
		if ([sender success])
		{
			[self setResultString:@"Completed successfully."];
			[self setResultIcon:[NSImage imageNamed:@"ok_16.tif"]];
			[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(addUserSheetCancelClicked:) userInfo:nil repeats:NO];
		}
		else
		{
			[self setResultString:@"Failed to save user details."];
			[self setResultIcon:[NSImage imageNamed:@"stop_16.tif"]];
		}
		updateXmlReq = nil;
	}
	if (sender == removeXmlReq) 
	{
		/* Remove Finished */
		if ([sender success])
		{
			[self setResultString:@"Completed successfully."];
			[self setResultIcon:[NSImage imageNamed:@"ok_16.tif"]];
			[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(removeUserSheetCancelClicked:) userInfo:nil repeats:NO];
		}
		else
		{
			[self setResultString:@"Failed to remove user."];
			[self setResultIcon:[NSImage imageNamed:@"stop_16.tif"]];
		}
		removeXmlReq = nil;
	}
	[self setEditUser:nil];
	[sender release];
	[[[setupController customer] userList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "Remove User"

- (IBAction) removeUserClicked:(id)sender
{
	/* Get Selection */
	if ([[userArrayController selectedObjects] count] < 1) return;
	[self setEditUser:[[userArrayController selectedObjects] objectAtIndex:0]];
	
	/* Open sheet */
	[self setResultString:@""];
	[self setResultIcon:nil];
	[NSApp beginSheet:removeUserSheet
	   modalForWindow:[setupController window]
		modalDelegate:self 
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (IBAction) removeUserSheetRemoveClicked:(id)sender
{
	/* Remove user from Core */
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"user"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"username" stringValue:[editUser username]]];
	
	/* Create and perform request */
	removeXmlReq = [[LCXMLRequest requestWithCriteria:[setupController customer]
											 resource:[[setupController customer] resourceAddress]
											   entity:[[setupController customer] entityAddress]
											  xmlname:@"user_remove"
											   refsec:0
											   xmlout:xmldoc] retain];
	[removeXmlReq setDelegate:self];
	[removeXmlReq setPriority:XMLREQ_PRIO_HIGH];
	[removeXmlReq performAsyncRequest];	
	[self setXmlOperationInProgress:YES];
	[self setResultString:@"Removing user..."];
	[self setResultIcon:nil];
}

- (IBAction) removeUserSheetCancelClicked:(id)sender
{
	[NSApp endSheet:removeUserSheet];
	[removeUserSheet close];
	[self setEditUser:nil];
}

#pragma mark "Accessors"

- (LCUser *) editUser
{ return editUser; }

- (void) setEditUser:(LCUser *)newUser
{
	[editUser release];
	editUser = [newUser retain];
}

- (NSString *) resultString
{ return resultString; }
- (void) setResultString:(NSString *)string
{ 
	[resultString release];
	resultString = [string retain];
}

- (NSImage *) resultIcon
{ return resultIcon; }
- (void) setResultIcon:(NSImage *)image
{
	[resultIcon release];
	resultIcon = [image retain];
}

- (BOOL) xmlOperationInProgress
{ return xmlOperationInProgress; }

- (void) setXmlOperationInProgress:(BOOL)value
{ xmlOperationInProgress = value; }

@synthesize setupController;
@synthesize addUserSheet;
@synthesize removeUserSheet;
@synthesize editGlobalAdminSheet;
@synthesize userArrayController;
@synthesize saveButton;
@synthesize usernameField;
@synthesize passwordField;
@synthesize updateXmlReq;
@synthesize removeXmlReq;
@synthesize xmlElement;
@synthesize xmlString;
@end
