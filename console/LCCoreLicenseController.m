//
//  LCCoreLicenseWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 6/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCoreLicenseController.h"

#import "LCDemoRegoWindowController.h"

@implementation LCCoreLicenseController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	/* Create lists */
	[self setKeyList:[LCCoreLicenseKeyList keyListForCustomer:[setupController customer]]];
	[self setEntitlement:[LCCoreLicenseEntitlement entitlementForCustomer:[setupController customer]]];;
	
	/* Start refresh */
	[keyList highPriorityRefresh];
	[entitlement highPriorityRefresh];
}

- (void) dealloc
{ 
	[keyList release];
	[entitlement release];
	[super dealloc]; 
}

#pragma mark "Add License Key"

- (IBAction) addLicenseKeyClicked:(id)sender
{
	/* Open sheet */
	[self setLicenseKeyString:@""];
	[self setSignedKeyString:@""];
	[self setAddResult:nil];
	[self setProgressString:@""];
	[self setErrorMessage:@""];
	[tabView selectTabViewItemAtIndex:0];
	[NSApp beginSheet:addKeySheet
	   modalForWindow:[setupController window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) closeAddLicenseKeySheet:(id)sender
{
	[NSApp endSheet:addKeySheet];
	[addKeySheet close];
}

- (IBAction) privacyClicked:(id)sender
{
	/* Open Privacy Info */
	NSURL *url = [NSURL URLWithString:@"http://secure.lithiumcorp.com.au/shop/help.php?section=business"];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction) addLicenseKeySheetClicked:(id)sender
{
	/* Check format of key */
	if ([[self licenseKeyString] length] > 30 && [[self licenseKeyString] length] < 50)
	{
		/* eKey requiring activation */
		[tabView selectTabViewItemAtIndex:1];		
	}
	else
	{
		/* Signed key does not need activation */
		[self setSignedKeyString:licenseKeyString];
		[self uploadSignedKeyToCore];
	}
}

- (IBAction) activateLicenseKeySheetClicked:(id)sender
{
	/* 
	 * Validate 
	 */
	
	BOOL valid = YES;
	
	if ([[firstnameTextField stringValue] length] < 1)
	{ [firstnameTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [firstnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[lastnameTextField stringValue] length] < 1)
	{ [lastnameTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [lastnameTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[companyTextField stringValue] length] < 1)
	{ [companyTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [companyTextField setBackgroundColor:[NSColor whiteColor]]; }
	if ([[emailTextField stringValue] length] < 1)
	{ [emailTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else if (!strstr([[emailTextField stringValue] UTF8String], "@"))
	{ [emailTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [emailTextField setBackgroundColor:[NSColor whiteColor]]; }
	
	if (!valid) return;
	
	[tabView selectTabViewItemAtIndex:2];
	
	[self activateLicenseKey];
}

- (void) activateLicenseKey
{
	/* Create URL String */
	NSURL *baseURL = [NSURL URLWithString:[[setupController customer] url]];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/activate_license.php?firstname=%@&lastname=%@&company=%@&email=%@&custname=%@&host=%@&key=%@", 
		[firstname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[lastname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[[[setupController customer] name] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[[baseURL host] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[licenseKeyString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	
	/* Setup URL request */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	/* Perform request */
	[self setXmlOperationInProgress:YES];
	[self setProgressString:@"Requesting License Activation..."];
	[self setErrorMessage:@""];
	[tabView selectTabViewItemAtIndex:2];
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
		[activity setStatus:@"Proceeding"];
	}
	else
	{ 
		/* Connection failed */
		[activity setStatus:@"Failed"];
		[self connection:nil didFailWithError:nil];
	}
}

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[receivedData setLength:0];
	[activity setStatus:@"Received non-data response"];
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
	[activity setStatus:@"Receiving data"];
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			   willSendRequest:(NSURLRequest *)request
			  redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
	[activity setStatus:@"Received re-direction"];
    return request;
}

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Demo key Download finished */
	
	/* Validate & Parse */
	NSString *xml = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xml hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
		
		/* Upload */
		if ([[self addResult] intValue] == 1)
		{
			/* Key was activated successfully */
			[self setProgressString:@"License Activated. Uploading Key to Lithium Core."];
			[self uploadSignedKeyToCore];
		}
		else
		{
			[self setXmlOperationInProgress:NO];
			[self setProgressString:@"License Key Activation Failed."];
		}	
	}
	else
	{
		[self setXmlOperationInProgress:NO];
		[self setProgressString:@"Error in response from Registration Server. Try again later."];
		[self setErrorMessage:@"If this problem persists, please email support@lithiumcorp.com."];
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Report status */
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Set status */
	[self setProgressString:@"Failed to contact registration server. Please try again later."]; 
	[self setErrorMessage:@"If this problem persists, please email support@lithiumcorp.com."];
	[self setXmlOperationInProgress:NO];
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set activity */
	[activity setStatus:@"Request Failed"];
	[activity invalidate];
	[activity release];
	activity = nil;
}

- (void) uploadSignedKeyToCore
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"license_keys"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"key" stringValue:[self signedKeyString]]];
	
	/* Create and perform request */
	addXMLRequest = [[LCXMLRequest requestWithCriteria:[setupController customer]
									   resource:[[setupController customer] resourceAddress]
										 entity:[[setupController customer] entityAddress]
										xmlname:@"lic_key_add"
										 refsec:0
										 xmlout:xmldoc] retain];
	[self setXmlOperationInProgress:YES];
	[addXMLRequest setDelegate:self];
	[addXMLRequest setXMLDelegate:self];
	[addXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[addXMLRequest performAsyncRequest];
	[self setProgressString:@"Uploading License Key to Lithium Core."];
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
		/*
		 * Update our properties 
		 */
		if ([xmlElement isEqualToString:@"message"])
		{ [self setErrorMessage:xmlString]; }
		else if ([xmlElement isEqualToString:@"result"])
		{ [self setAddResult:xmlString]; }
		else if ([xmlElement isEqualToString:@"key"])
		{ [self setSignedKeyString:xmlString]; }
	}
	
	/* Release current element */
	[xmlElement release];
	xmlElement = nil;
	[xmlString release];
	xmlString = nil;
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Set flag */
	[self setXmlOperationInProgress:NO];
	
	/* Check op */
	if (sender == addXMLRequest)
	{
		/* Check success */
		if ([addResult intValue] == 0)
		{
			/* Other error occurred */
			[self setAddResult:@"2"];
			[self setProgressString:@"Failed to upload License Key to Lithium Core."];
		}

		/* Timed close */
		if ([addResult intValue] == 1)
		{ 
			/* OK */
			[tabView selectTabViewItemAtIndex:3];
		}
		
		addXMLRequest = nil;
	}
	else if (sender == removeXMLRequest)
	{
		/* Timed close */
		if ([sender success])
		{
			[self setProgressString:@"License key removed."];
			if ([[keyArrayController selectedObjects] count] > 0)
			{ 
				LCCoreLicenseKey *key = [[keyArrayController selectedObjects] objectAtIndex:0];
				[keyList removeObjectFromKeysAtIndex:[[keyList keys] indexOfObject:key]];
			}
			[NSTimer scheduledTimerWithTimeInterval:0.0 
											 target:self 
										   selector:@selector(removeSheetCancelClicked:) 
										   userInfo:nil 
											repeats:NO];
		}
		else
		{ [self setProgressString:@"Failed to remove license key."]; }
		
		removeXMLRequest = nil;
	}
	
	/* Release request */
	[sender release];
	
	/* Refresh */
	[[setupController customer] highPriorityRefresh];
	[keyList highPriorityRefresh];	
	[entitlement highPriorityRefresh];
}

#pragma mark "Remove License Key"

- (IBAction) removeLicenseKeyClicked:(id)sender
{
	/* Open sheet */
	[self setProgressString:@""];
	[NSApp beginSheet:removeKeySheet
	   modalForWindow:[setupController window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];			
}

- (IBAction) removeSheetRemoveClicked:(id)sender
{
	/* Check State */
	if (xmlOperationInProgress) return;
	if ([[keyArrayController selectedObjects] count] < 1) return;
	
	/* Get selected */
	LCCoreLicenseKey *selectedKey = [[keyArrayController selectedObjects] objectAtIndex:0];
	
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"license_keys"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"id" stringValue:[NSString stringWithFormat:@"%i", selectedKey.keyID]]];
	
	/* Create and perform request */
	removeXMLRequest = [[LCXMLRequest requestWithCriteria:[setupController customer]
												 resource:[[setupController customer] resourceAddress]
												   entity:[[setupController customer] entityAddress]
												  xmlname:@"lic_key_remove"
												   refsec:0
												   xmlout:xmldoc] retain];
	[self setXmlOperationInProgress:YES];
	[removeXMLRequest setDelegate:self];
	[removeXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[removeXMLRequest performAsyncRequest];
	[self setProgressString:@"Removing license key..."];
}

- (IBAction) removeSheetCancelClicked:(id)sender
{
	[NSApp endSheet:removeKeySheet];
	[removeKeySheet close];
}

#pragma mark "Purchase Online Clicked"

- (IBAction) purchaseOnlineClicked:(id)sender
{
	NSURL *url = [NSURL URLWithString:@"http://secure.lithiumcorp.com/shop"];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

#pragma mark "Get Demo License Clicked"

- (IBAction) getDemoLicenseClicked:(id)sender
{
	LCDemoRegoWindowController *controller = (LCDemoRegoWindowController *) [[LCDemoRegoWindowController alloc] initForCustomer:[setupController customer]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[setupController window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

#pragma mark "Accessors"

@synthesize keyList;
@synthesize entitlement;
@synthesize licenseKeyString;
@synthesize signedKeyString;
@synthesize xmlOperationInProgress;
@synthesize progressString;
@synthesize errorMessage;
@synthesize addResult;
@synthesize firstname;
@synthesize lastname;
@synthesize company;
@synthesize email;

@end
