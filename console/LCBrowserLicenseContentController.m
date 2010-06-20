//
//  LCBrowserLicenseContentController.m
//  Lithium Console
//
//  Created by James Wilson on 8/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserLicenseContentController.h"

#import "LCBrowser2ContentViewController.h"
#import "LCCustomer.h"
#import "LCDemoRegoWindowController.h"

@implementation LCBrowserLicenseContentController

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"LicenseContent" bundle:nil];
	if (!self) return nil;
	
	/* Set properties */
	self.customer = initCustomer;
	self.browser = initBrowser;
	self.keyList = [LCCoreLicenseKeyList keyListForCustomer:self.customer];
	self.entitlement = [LCCoreLicenseEntitlement entitlementForCustomer:self.customer];
	[self.keyList highPriorityRefresh];
	[self.entitlement highPriorityRefresh];
	
	/* Load NIB */
	[self loadView];
	
	/* Observe Selection */
	[keyArrayController addObserver:self 
						 forKeyPath:@"selection" 
							options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							context:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[keyArrayController removeObserver:self forKeyPath:@"selection"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[selectedKeys release];
	[keyList release];
	[entitlement release];
	[licenseKeyString release];
	[signedKeyString release];
	[firstname release];
	[lastname release];
	[company release];
	[email release];
	if (addXMLRequest)
	{
		[addXMLRequest cancel];
		[addXMLRequest release];
	}
	if (removeXMLRequest)
	{
		[removeXMLRequest cancel];
		[removeXMLRequest release];
	}
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedKey;
@synthesize selectedKeys;
- (void) setSelectedKeys:(NSArray *)value
{
	[selectedKeys release];
	selectedKeys = [value copy];
	
	if (selectedKeys.count > 0) self.selectedKey = [selectedKeys objectAtIndex:0];
	else self.selectedKey = nil;
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == keyArrayController)
	{
		self.selectedKeys = [keyArrayController selectedObjects];
	}
}

#pragma mark "Misc UI Actions"

- (IBAction) refreshClicked:(id)sender
{
	[entitlement highPriorityRefresh];
	[keyList highPriorityRefresh];
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
	   modalForWindow:[browser window]
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
	else if ([[self licenseKeyString] length] >= 50)
	{
		/* Signed key does not need activation */
		[self setSignedKeyString:licenseKeyString];
		[self uploadSignedKeyToCore];
	}
	else 
	{
		/* Invalid size */
		NSAlert *alert = [NSAlert alertWithMessageText:@"Invalid Key"
										 defaultButton:@"Try Again"
									   alternateButton:@"Cancel"
										   otherButton:nil
							 informativeTextWithFormat:@"The key you have entered is invalid."];
		[NSApp endSheet:addKeySheet];
		[addKeySheet close];
		[alert setAlertStyle:NSCriticalAlertStyle];
		[alert beginSheetModalForWindow:[browser window]
						  modalDelegate:self
						 didEndSelector:@selector(addKeyErrorSheetDidEnd:returnCode:contextInfo:)
							contextInfo:nil];
	}

}

- (void) addKeyErrorSheetDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Re-display the addKeySheet right where it left off */
		[[alert window] orderOut:self];
		[NSApp beginSheet:addKeySheet
		   modalForWindow:[browser window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
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
	NSURL *baseURL = [NSURL URLWithString:customer.url];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/activate_license5.php?firstname=%@&lastname=%@&company=%@&email=%@&custname=%@&host=%@&key=%@", 
						   [firstname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [lastname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [customer.name stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
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
	addXMLRequest = [[LCXMLRequest requestWithCriteria:customer
											  resource:[customer resourceAddress]
												entity:[customer entityAddress]
											   xmlname:@"lic_key_add"
												refsec:0
												xmlout:xmldoc] retain];
	[self setXmlOperationInProgress:YES];
	[addXMLRequest setDelegate:self];
	[addXMLRequest setXMLDelegate:self];	/* Use the same parser as used for Vince interaction */
	[addXMLRequest setPriority:XMLREQ_PRIO_HIGH];
	[addXMLRequest performAsyncRequest];
	[self setProgressString:@"Uploading License Key to Lithium Core."];
}

#pragma mark "Remove License Key"

- (IBAction) removeLicenseKeyClicked:(id)sender
{
	NSAlert *alert = [NSAlert alertWithMessageText:@"Confirm License Key Removal"
									 defaultButton:@"Delete"
								   alternateButton:@"Cancel"
									   otherButton:nil
						 informativeTextWithFormat:@"This action can not be undone"];
	[alert beginSheetModalForWindow:[browser window]
					  modalDelegate:self
					 didEndSelector:@selector(removeKeyAlertDidEnd:returnCode:contextInfo:)
						contextInfo:nil];
}

- (void) removeKeyAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOKButton)
	{
		/* Check State */
		if (xmlOperationInProgress) return;
		if ([[keyArrayController selectedObjects] count] < 1) return;
		
		/* Get selected */
		LCCoreLicenseKey *key = [[keyArrayController selectedObjects] objectAtIndex:0];
		
		/* Create XML */
		NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"license_keys"];
		NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
		[xmldoc setVersion:@"1.0"];
		[xmldoc setCharacterEncoding:@"UTF-8"];
		[rootnode addChild:[NSXMLNode elementWithName:@"id" stringValue:[NSString stringWithFormat:@"%i", key.keyID]]];
		
		/* Create and perform request */
		removeXMLRequest = [[LCXMLRequest requestWithCriteria:customer
													 resource:[customer resourceAddress]
													   entity:[[customer customer] entityAddress]
													  xmlname:@"lic_key_remove"
													   refsec:0
													   xmlout:xmldoc] retain];
		[removeXMLRequest setDelegate:self];
		[removeXMLRequest setPriority:XMLREQ_PRIO_HIGH];
		[removeXMLRequest performAsyncRequest];
		
		/* Remove key from list */
		[keyList removeObjectFromKeysAtIndex:[[keyList keys] indexOfObject:key]];		
	}
}

#pragma mark "XML Parser (Vince Interaction) Delegate Methods"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
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
	if (xmlString) { [xmlString appendString:string]; }
	else { xmlString = [[NSMutableString stringWithString:string] retain]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Update properties */
	if (xmlString)
	{ 
		/*
		 * Update our properties 
		 */
		if ([element isEqualToString:@"message"])
		{ [self setErrorMessage:xmlString]; }
		else if ([element isEqualToString:@"result"])
		{ [self setAddResult:xmlString]; }
		else if ([element isEqualToString:@"key"])
		{ [self setSignedKeyString:xmlString]; }
	}
	
	/* Release current element */
	[xmlString release];
	xmlString = nil;
}

#pragma mark "XML Parser (Core Interaction) Delegate Methods"

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Set flag */
	self.xmlOperationInProgress = NO;
	
	/* Check op */
	if (sender == addXMLRequest)
	{
		/* Check success */
		if ([addResult intValue] == 0)
		{
			/* Other error occurred */
			self.addResult = @"2";
			self.progressString = @"Failed to upload License Key to Lithium Core.";
		}
		
		/* Timed close */
		if ([addResult intValue] == 1)
		{ 
			/* OK */
			[tabView selectTabViewItemAtIndex:3];
		}

		[addXMLRequest release];
		addXMLRequest = nil;
	}
	else if (sender == removeXMLRequest)
	{
		/* Timed close */
		[removeXMLRequest release];
		removeXMLRequest = nil;
	}
	
	/* Refresh */
	[customer highPriorityRefresh];
	[keyList highPriorityRefresh];	
	[entitlement highPriorityRefresh];
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
	LCDemoRegoWindowController *controller = (LCDemoRegoWindowController *) [[LCDemoRegoWindowController alloc] initForCustomer:customer];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[browser window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
//	SEL action = [item action];
	
	return NO;
}

#pragma mark "Properties"

@synthesize browser;
@synthesize customer;
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
