//
//  LCBrowserUsersContentController.m
//  Lithium Console
//
//  Created by James Wilson on 5/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserUsersContentController.h"


@implementation LCBrowserUsersContentController

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"UsersContent" bundle:nil];
	if (!self) return nil;
	
	/* Set properties */
	self.customer = initCustomer;
	self.browser = initBrowser;
	
	/* Load NIB */
	[self loadView];
	
	/* Observe Selection */
	[userArrayController addObserver:self 
						  forKeyPath:@"selection" 
							 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							 context:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[userArrayController removeObserver:self forKeyPath:@"selection"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[selectedUsers release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedUser;
@synthesize selectedUsers;
- (void) setSelectedUsers:(NSArray *)value
{
	[selectedUsers release];
	selectedUsers = [value copy];
	
	if (selectedUsers.count > 0) self.selectedUser = [selectedUsers objectAtIndex:0];
	else self.selectedUser = nil;
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == userArrayController)
	{
		self.selectedUsers = [userArrayController selectedObjects];
	}
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
//	SEL action = [item action];
	
	return NO;
}

- (BOOL) validateUserInput
{	
	/* Reset */
	self.editSheetStatus = nil;
	[editSheetUsername setBackgroundColor:[NSColor whiteColor]];
	[editSheetPassword setBackgroundColor:[NSColor whiteColor]];
	[editSheetConfirmPassword setBackgroundColor:[NSColor whiteColor]];
	[editSheetFullname setBackgroundColor:[NSColor whiteColor]];
	[editSheetTitle setBackgroundColor:[NSColor whiteColor]];
	[editSheetEmail setBackgroundColor:[NSColor whiteColor]];

	/* Check password */
	if (!editUser.password || [editUser.password length] < 1) 
	{ 
		self.editSheetStatus = @"A password must be set"; 
		[editSheetPassword setBackgroundColor:[NSColor redColor]];
		return NO;
	}
	if (![editUser.password isEqualToString:editUser.confirmPassword])
	{
		self.editSheetStatus = @"Passwords do not match";
		[editSheetPassword setBackgroundColor:[NSColor redColor]];
		[editSheetConfirmPassword setBackgroundColor:[NSColor redColor]];
		return NO;
	}
	
	return YES;
}

#pragma mark "UI Actions"

- (IBAction) addNewUserClicked:(id)sender
{
	self.editSheetStatus = nil;
	self.editUser = [[LCUser new] autorelease];
	self.editUser.accessLevel = 30;
	self.editUser.customer = customer;
	self.editUser.delegate = self;
	self.editUserIsNew = YES;
	
	[editSheetLabel setStringValue:@"Add New User"];
	[editSheetOKButton setTitle:@"Add"];
	
	[editSheet makeFirstResponder:editSheetUsername];
	[NSApp beginSheet:editSheet
	   modalForWindow:[browser window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) editSelectedUserClicked:(id)sender
{
	if (!self.selectedUser) return;
	self.editSheetStatus = nil;
	self.editUser = [self.selectedUser mutableCopy];
	self.editUser.customer = customer;
	self.editUser.delegate = self;
	self.editUserIsNew = NO;
	[editSheetLabel setStringValue:@"Edit User"];
	[editSheetOKButton setTitle:@"Save"];
	
	[editSheet makeFirstResponder:editSheetFullname];
	[NSApp beginSheet:editSheet
	   modalForWindow:[browser window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];	
}

- (IBAction) editSheetOKClicked:(id)sender
{
	/* Validate */
	if (![self validateUserInput]) return;
	
	if (editUserIsNew){
		[editUser performXmlAdd];
		self.editSheetStatus = @"Adding User...";
	}
	else {
		[editUser performXmlUpdate];
		self.editSheetStatus = @"Updating User...";
	}
	
	self.xmlOperationInProgress = YES;
}

- (IBAction) editSheetCancelClicked:(id)sender
{
	[NSApp endSheet:editSheet];
	[editSheet close];
}

- (IBAction) deleteSelectedUserClicked:(id)sender
{
	if (!self.selectedUser) return;
	self.editUser = [self.selectedUser mutableCopy];
	self.editUser.customer = customer;
	self.editUser.delegate = self;
	self.editUserIsNew = NO;

	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm User Delete"
									 defaultButton:@"Delete" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert beginSheetModalForWindow:[browser window]
					  modalDelegate:self
					 didEndSelector:@selector(deleteUserSheetEnded:returnCode:contextInfo:)
						contextInfo:nil];
	
}

- (void) deleteUserSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		[editUser performXmlDelete];
		LCUser *existingUser = [self.customer.userList.userDictionary objectForKey:editUser.username];
		if (existingUser)
		{ [customer.userList removeObjectFromUsersAtIndex:[customer.userList.users indexOfObject:existingUser]]; }
		self.xmlOperationInProgress = YES;
	}
}

- (IBAction) userTableDoubleClicked:(NSArray *)tableSelectedObjects
{
	[self editSelectedUserClicked:self];
}

- (IBAction) refreshUserListClicked:(id)sender
{
	[[customer userList] refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "User Delegate Methods"

- (void) xmlObjectAddFinished:(LCUser *)user
{
	if (user.xmlOperationSuccess)
	{
		[self.customer.userList insertObject:user inUsersAtIndex:self.customer.userList.users.count];
		self.editUser = nil;
		self.editSheetStatus = @"Done";
		[NSApp endSheet:editSheet];
		[editSheet close];	
	}
	else
	{
		self.editSheetStatus = @"Failed to add user.";
	}
	self.xmlOperationInProgress = NO;
}

- (void) xmlObjectUpdateFinished:(LCUser *)user
{
	if (user.xmlOperationSuccess)
	{
		LCUser *existingUser = [self.customer.userList.userDictionary objectForKey:user.username];
		[existingUser copyXmlPropertiesFromObject:user];
		self.editUser = nil;
		self.editSheetStatus = @"Done";
		[NSApp endSheet:editSheet];
		[editSheet close];		
	}
	else
	{
		self.editSheetStatus = @"Failed to update user.";
	}	
	self.xmlOperationInProgress = NO;
}

#pragma mark "Properties"

@synthesize browser;
@synthesize customer;
@synthesize editUser;
@synthesize editUserIsNew;
@synthesize editSheetStatus;
@synthesize xmlOperationInProgress;

@end
