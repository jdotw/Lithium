//
//  LCSiteEditController.m
//  Lithium Console
//
//  Created by James Wilson on 27/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCSiteEditController.h"


@implementation LCSiteEditController

#pragma mark "Constructors"

- (LCSiteEditController *) initForNewSiteAtCustomer:(LCCustomer *)initCustomer
{
	[self init];
	
	self.customer = initCustomer;

	CFUUIDRef uuidRef = CFUUIDCreate(nil);
	self.name = (NSString *)CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);	

	[addButton setTitle:@"Add"];
	
	return self;
}

- (LCSiteEditController *) initWithSiteToEdit:(LCSite *)initSite
{
	[self init];
	
	self.siteEntity = initSite;
	self.customer = [initSite customer];
	[addButton setTitle:@"Save"];
	
	return self;
}

- (LCSiteEditController *) initWithSiteToRemove:(LCEntity *)initSite windowForSheet:(NSWindow *)initWindowForSheet
{
	[self init];
	
	self.siteEntity = (LCSite *) initSite;
	self.customer = initSite.customer;
	self.windowForSheet = initWindowForSheet;
	
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm Site Delete"
									 defaultButton:@"Delete" 
								   alternateButton:@"Cancel" 
									   otherButton:nil 
						 informativeTextWithFormat:@"This action can not be undone."];
	[alert beginSheetModalForWindow:initWindowForSheet
					  modalDelegate:self
					 didEndSelector:@selector(removeSiteSheetEnded:returnCode:contextInfo:)
						contextInfo:nil];
	
	return self;
	
}

- (LCSiteEditController *) init
{
	[super initWithWindowNibName:@"SiteEditWindow"];
	
	/* Load NIB */
	[self window];
	
	return self;
}

- (void) dealloc
{
	[siteEntity release];
	[customer release];
	[name release];
	[desc release];
	[addressLine1 release];
	[addressLine2 release];
	[addressLine3 release];
	[suburb release];
	[state release];
	[postcode release];
	[country release];

	[super dealloc];
}

#pragma mark "UI Actions"

- (IBAction) addClicked:(id)sender
{
	/* Validate */
	if (![self validateInput]) return;
	
	/* Set Status */
	self.statusIcon = nil;
	self.statusString = nil;
	
	/* Check to see if we're updating a live/existing site */
	if (siteEntity)
	{
		[self updateLiveSite:siteEntity];
	}
	
	/* Perform */
	[self performUpdate];
}

- (IBAction) cancelClicked:(id)sender
{
	/* Cancel XML */
	[xmlRequest cancel];
	[xmlRequest release];
	
	/* Close */
	[self closeSheet];
}

- (void) closeSheet
{
	[NSApp endSheet:[self window]];
	[[self window] close];
}

#pragma mark "UI Variable Accessors"
@synthesize windowForSheet;

#pragma mark "Validation"

- (BOOL) validateInput
{
	/* Name */
	if (!self.name || [self.name length] < 1)
	{
		self.statusString = @"Error: Site 'short name' required";
		self.statusIcon = [NSImage imageNamed:@"stop_16.tif"];
		return NO;
	}
	
	/* Desc */
	if (!self.desc || [self.desc length] < 1)
	{
		self.statusString = @"Error: Site description required";
		self.statusIcon = [NSImage imageNamed:@"stop_16.tif"];
		return NO;
	}
	
	return YES;
}

#pragma mark "Device Removal Methods"

- (void) removeSiteSheetEnded:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Remove site */
		[[alert window] orderOut:self];
		[NSApp beginSheet:removeProgressSheet
		   modalForWindow:windowForSheet 
			modalDelegate:self 
		   didEndSelector:nil 
			  contextInfo:nil];
		[self performRemove];
	}
}

- (IBAction) removeSiteCancelClicked:(id)sender
{
	[NSApp endSheet:removeProgressSheet];
	[removeProgressSheet close];
	[removeXMLReq cancel];
	[removeXMLReq release];
}

#pragma mark "XML Operation Methods"

- (LCXMLRequest *) performUpdate
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"site"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	NSDictionary *siteProperties = [self siteProperties];
	for (NSString *key in siteProperties)
	{ 
		[rootnode addChild:[NSXMLNode elementWithName:key stringValue:[[siteProperties objectForKey:key] description]]]; 
	}
	
	/* Create and perform request */
	xmlRequest = [[LCXMLRequest requestWithCriteria:customer
										   resource:[customer resourceAddress]
											 entity:[customer entityAddress]
											xmlname:@"site_update"
											 refsec:0
											 xmlout:xmldoc] retain];		
	[xmlRequest setDelegate:self];
	[xmlRequest setXMLDelegate:self];
	[xmlRequest setPriority:XMLREQ_PRIO_HIGH];
	[xmlRequest performAsyncRequest];
	
	[self setXmlOperationInProgress:YES];
	
	return xmlRequest;
}

- (LCXMLRequest *) performRemove
{
	/* Update the device */
	NSXMLDocument *xmldoc;
	NSXMLElement *rootnode;
	
	/* Create XML */
	rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"site"];
	xmldoc = [NSXMLDocument documentWithRootElement:rootnode];
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"name" stringValue:[siteEntity name]]];
	
	/* Create and perform request */
	removeXMLReq = [[LCXMLRequest requestWithCriteria:customer
											 resource:[customer resourceAddress]
											   entity:[customer entityAddress]
											  xmlname:@"site_remove"
											   refsec:0
											   xmlout:xmldoc] retain];	
	[removeXMLReq setDelegate:self];
	[removeXMLReq setPriority:XMLREQ_PRIO_HIGH];
	[removeXMLReq performAsyncRequest];

	[self setXmlOperationInProgress:YES];

	return removeXMLReq;
}

#pragma mark "XML Request Delegate Methods"

- (void) XMLRequestPreParse:(LCXMLRequest *)sender
{
	xmlProperties = [[NSMutableDictionary dictionary] retain]; 
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Check to see if this was a remove */
	if (sender == removeXMLReq)
	{
		/* Hide progress sheet and remove device */
		[self setXmlOperationInProgress:NO];
		[NSApp endSheet:removeProgressSheet];
		[removeProgressSheet close];
		[customer removeChild:siteEntity];
		[customer highPriorityRefresh];
		[self autorelease];
		[removeXMLReq release];
		removeXMLReq = nil;
		return;
	}
	
	/* Set flags */
	xmlRequest = nil;
	[self setXmlOperationInProgress:NO];
	
	/* Check something was received */
	if ([sender success] && xmlProperties && [xmlProperties count] > 0)
	{
		/* Check for entity */
		if (siteEntity)
		{
			/* This was an update operation, update existing entity */
			for (NSString *key in [xmlProperties allKeys])
			{
				[siteEntity setXmlValue:[xmlProperties objectForKey:key] forKey:key];
			}
			[siteEntity highPriorityRefresh];				
		}
		else
		{
			/* Create entity using received properties */
			if (xmlProperties)
			{
				LCSite *site = (LCSite *) [[LCSite alloc] initWithType:2
																  name:[xmlProperties objectForKey:@"name"] 
																  desc:[xmlProperties objectForKey:@"desc"] 
												 resourceAddressString:[xmlProperties objectForKey:@"resaddr"]];
				[site setParent:customer];
				[site setPresenceConfirmed:YES];
				[customer insertObject:site inChildrenAtIndex:[[customer children] count]];
				[site highPriorityRefresh];
				[site autorelease];
			}
		}			
	}
	else
	{
		/* Error occurred */
		errorEncountered = YES;
	}
	
	/* All Finished */
	[self setXmlOperationInProgress:NO];
	if (errorEncountered)
	{
		if (!siteEntity) [self setStatusString:@"Failed to add site"];
		else [self setStatusString:@"Failed to update site"];
		[self setStatusIcon:[NSImage imageNamed:@"stop_16.tif"]];
	}
	else
	{
		if (!siteEntity) [self setStatusString:@"Site added successfully"];
		else [self setStatusString:@"Site updated successfully"];
		[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(closeSheet) 
										   userInfo:nil repeats:NO];
		[self setStatusIcon:[NSImage imageNamed:@"ok_16.tif"]];
	}

	/* Clear variables */
	[xmlProperties release];
	xmlProperties = nil;
	
	/* Release request */
	[sender release];
}

#pragma mark "XML Parser Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Retain element */
	xmlElement = [element retain];
	
	/* Release previous string */
	if (xmlString)
	{
		[xmlString release];
		xmlString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Create new string or append string to existing */
	if (xmlElement)
	{
		if (xmlString) { [xmlString appendString:string]; }
		else { xmlString = [[NSMutableString stringWithString:string] retain]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlElement && xmlString)
	{ 
		[xmlProperties setObject:xmlString forKey:xmlElement]; 
		[xmlString release];
		xmlString = nil;
	}
	
	/* Release current element */
	if (xmlElement)
	{
		[xmlElement release];
		xmlElement = nil;
	}
}

#pragma mark "Site Property Methods"

@synthesize name;
@synthesize desc;
@synthesize addressLine1;
@synthesize addressLine2;
@synthesize addressLine3;
@synthesize suburb;
@synthesize state;
@synthesize postcode;
@synthesize country;
@synthesize longitude;
@synthesize latitude;

- (void) updateLiveSite:(LCSite *)site
{
	/* Updated the live (real) LCSite entity with
	 * the the update controllers local values.
	 * 
	 * This is done before an update/submit operation
	 */
	
	[site copyXmlPropertiesFromObject:self];
}

- (NSMutableDictionary *) siteProperties
{
	/* Combines all the properties that may be required
	 * for a add/test/edit/remove operation into 
	 * a NSMutableDictionary
	 */
	
	LCSite *site;
	
	if (siteEntity)
	{
		site = siteEntity;
	}
	else
	{
		site = [[[LCSite alloc] init] autorelease];
	}
	
	[site copyXmlPropertiesFromObject:self];
	NSMutableDictionary *siteProperties = [NSMutableDictionary dictionaryWithDictionary:[site xmlPropertiesDictionary]];
	
	return siteProperties;
}

- (void) setValue:(id)value forUndefinedKey:(NSString *)key
{
	
}

#pragma mark "Property Methods"

@synthesize siteEntity;
- (void) setSiteEntity:(LCSite *)value
{
	[siteEntity release];
	siteEntity = [value retain];
	
	[siteEntity copyXmlPropertiesToObject:self];
	
}
@synthesize customer;
@synthesize statusString;
@synthesize statusIcon;
@synthesize xmlOperationInProgress;

@end
