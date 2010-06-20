//
//  LCProcessProfileEditWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 15/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCProcessProfileEditWindowController.h"
#import "LCCustomer.h"

@interface LCProcessProfileEditWindowController (private)
- (void) performXmlAction:(NSString *)xmlName;
- (void) updateLiveProfile;
@end


@implementation LCProcessProfileEditWindowController

#pragma mark "Constructors"

- (id) initForNewProfileMatch:(NSString *)initMatch device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"ProcessProfileEditWindow" owner:self];
	if (!self) return nil;
	[self loadWindow];
	
	self.device = initDevice;
	self.windowForSheet = initWindow;
	self.profile = [[LCProcessProfile new] autorelease];
	if (initMatch) 
	{
		self.desc = initMatch;
		self.match = initMatch;
	}
	addProfile = YES;
	
	[titleTextField setStringValue:@"Add Process Monitoring Profile"];
	[okButton setTitle:@"Add"];
	
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	sheetShown = YES;
	
	return self;
}

- (id) initWithProfileToEdit:(LCProcessProfile *)initProfile device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"ProcessProfileEditWindow" owner:self];
	if (!self) return nil;
	
	self.device = initDevice;
	self.profile = initProfile;
	self.desc = self.profile.desc;
	self.match = self.profile.match;
	self.argumentsMatch = self.profile.argumentsMatch;
	self.windowForSheet = initWindow;
	[self loadWindow];
	
	[titleTextField setStringValue:@"Edit Process Monitoring Profile"];
	[okButton setTitle:@"Save"];
	
	[NSApp beginSheet:[self window]
	   modalForWindow:windowForSheet
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
	sheetShown = YES;
	
	return self;
}

- (id) initWithProfileToDelete:(LCProcessProfile *)initProfile device:(LCDevice *)initDevice windowForSheet:(NSWindow *)initWindow
{
	self = [self initWithWindowNibName:@"ProcessProfileEditWindow" owner:self];
	if (!self) return nil;
	
	self.device = initDevice;
	self.profile = initProfile;
	self.windowForSheet = initWindow;
	removeProfile = YES;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Process Monitoring Profile Removal"
									 defaultButton:@"Remove"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert beginSheetModalForWindow:self.windowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(removeProfileAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
	
	return self;	
}

- (void) dealloc
{
	[profile release];
	[desc release];
	[match release];
	[argumentsMatch release];
	[windowForSheet release];
	[super dealloc];
}

#pragma mark "UI Actions"

- (IBAction) saveClicked:(id)sender
{
	[self updateLiveProfile];
	if (profile.profileID == 0)
	{ [self performXmlAction:@"procpro_add"]; }
	else
	{ [self performXmlAction:@"procpro_update"]; }	
}

- (IBAction) cancelClicked:(id)sender
{
	[NSApp endSheet:[self window]];
	[[self window] close];
	[self autorelease];	
}

- (void) removeProfileAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Perform delete */
		[self performXmlAction:@"procpro_delete"];
	}
}

#pragma mark "XML Methods"

- (void) updateLiveProfile
{
	/* Updates the live LCProcessProfile based on the user input */
	self.profile.desc = self.desc;
	self.profile.match = self.match;
	self.profile.argumentsMatch = self.argumentsMatch;
}

- (void) performXmlAction:(NSString *)xmlName
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"procpro"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"desc" 
									  stringValue:self.profile.desc]];
	[rootnode addChild:[NSXMLNode elementWithName:@"match" 
									  stringValue:self.profile.match]];
	[rootnode addChild:[NSXMLNode elementWithName:@"argmatch" 
									  stringValue:self.profile.argumentsMatch]];
	if (self.profile.profileID > 0)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"id"
										  stringValue:[NSString stringWithFormat:@"%i", self.profile.profileID]]];
	}
	
	/* Create and perform request */
	xmlRequest = [[LCXMLRequest requestWithCriteria:device.customer
										   resource:[device resourceAddress]
											 entity:[device entityAddress]
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
		self.profile.profileID = [[rootNode.properties objectForKey:@"id"] intValue];
		shouldClose = YES;
		if (addProfile)
		{
			[device insertObject:profile inProcProfilesAtIndex:device.procProfiles.count];
		}
	}
	else
	{
		/* Neither an error nor an ID received */
		self.status = @"ERROR: Server did not respond.";
		shouldClose = NO;
	}
	
	if (removeProfile)
	{
		[device removeObjectFromProcProfilesAtIndex:[device.procProfiles indexOfObject:profile]];
	}
	
	[device highPriorityRefresh];
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Update status */
	self.xmlOperationInProgress = NO;
	if (![sender success])
	{ self.status = @"Failed to send request."; }
	
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
	
	/* Update process list */
	[[[device customer] processProfileList] highPriorityRefresh];
	
	/* Cleanup */
	[xmlRequest release];
	xmlRequest = nil;
}

#pragma mark "Properties"
@synthesize profile;
@synthesize device;
@synthesize windowForSheet;
@synthesize desc;
@synthesize match;
@synthesize argumentsMatch;
@synthesize xmlOperationInProgress;
@synthesize status;

@end
