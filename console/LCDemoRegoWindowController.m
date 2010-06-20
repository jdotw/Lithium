//
//  LCDemoRegoWindowController.m
//  Lithium Console
//
//  Created by James Wilson on 2/12/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCDemoRegoWindowController.h"


@implementation LCDemoRegoWindowController

#pragma mark "Initialisation"

- (id) initForCustomer:(LCCustomer *)initCustomer
{
	/* Set variables */
	[self setCustomer:initCustomer];
	
	/* Load NIB */
	[super initWithWindowNibName:@"DemoRegistrationWindow"];
	[self window];
	
	return self;
}

#pragma mark "UI Actions"

- (IBAction) registerClicked:(id)sender
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
	else if (!strstr([[emailTextField stringValue] cStringUsingEncoding:NSUTF8StringEncoding], "@"))
	{ [emailTextField setBackgroundColor:[NSColor redColor]]; valid = NO; }
	else
	{ [emailTextField setBackgroundColor:[NSColor whiteColor]]; }

	if (licenseIndex == 0)
	{ [usageTextField setTextColor:[NSColor redColor]]; valid = NO; }
	else
	{ [usageTextField setTextColor:[NSColor blackColor]]; }
	
	if (!valid) return;
	
	/* Send rego */
	[self sendRegistration];
}

- (IBAction) cancelClicked:(id)sender
{
	/* Close */
	[NSApp endSheet:[self window]];
	[[self window] close];
	[controllerAlias setContent:nil];
	[self autorelease];
}

- (IBAction) privacyClicked:(id)sender
{
	/* Open Privacy Info */
	NSURL *url = [NSURL URLWithString:@"http://secure.lithiumcorp.com.au/shop/help.php?section=business"];
	[[NSWorkspace sharedWorkspace] openURL:url];
}

#pragma mark "Send Registration"

- (void) sendRegistration
{
	/* Create URL String */
	NSURL *baseURL = [NSURL URLWithString:[customer url]];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/register_demo5.php?firstname=%@&lastname=%@&company=%@&email=%@&licindex=%i&custname=%@&host=%@", 
		[firstname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[lastname stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding], licenseIndex, 
		[[customer name] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
		[[baseURL host] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	
	/* Setup URL request */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
														   cachePolicy:NSURLRequestReloadIgnoringCacheData
													   timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	/* Perform request */
	[self setRegoInProgress:YES];
	[self setProgressString:@"Downloading License Key."];
	[self setRegoMessage:@""];
	[tabView selectTabViewItemAtIndex:1];
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

#pragma mark HTTP Handling

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
	NSString *xmlString = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xmlString hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
	}	
	else
	{
		[self setRegoInProgress:NO];
		[self setProgressString:@"Error in response from Registration Server. Try again later."];
		[self setRegoMessage:@"If this problem persists, please email support@lithiumcorp.com to request a license key."];
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
	[self setRegoInProgress:NO];
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set activity */
	[activity setStatus:@"Request Failed"];
	[activity invalidate];
	[activity release];
	activity = nil;
}

#pragma mark "XML Parsing -- From Vince"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	if (curXMLElement)
	{
		if (!curXMLString)
		{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
		else
		{ [curXMLString appendString:string]; }
	}
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Check for result and message */
	if ([element isEqualToString:@"result"])
	{ 
		if ([curXMLString intValue] == 1)
		{
			/* Registration Successful */
			[self uploadLicenseKey];
			[self setProgressString:@"License Key Received. Uploading to Lithium Core."];
		}
		else
		{
			/* Reigstration Failed */
			[self setProgressString:@"Demo Registration Failed."];
			[self setRegoInProgress:NO];
		}
	}
	else if ([element isEqualToString:@"message"])
	{ 
		[self setRegoMessage:curXMLString];
	}
	
	/* Check for key */
	else if ([element isEqualToString:@"key"])
	{ 
		[self setLicenseKey:curXMLString];
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
}

#pragma mark "Upload to License Key -- To Core"

- (void) uploadLicenseKey
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"license_keys"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	[rootnode addChild:[NSXMLNode elementWithName:@"key" stringValue:[self licenseKey]]];
	
	/* Create and perform request */
	xmlReq = [[LCXMLRequest requestWithCriteria:customer
									   resource:[customer resourceAddress]
										 entity:[customer entityAddress]
										xmlname:@"lic_key_add"
										 refsec:0
										 xmlout:xmldoc] retain];
	[xmlReq setDelegate:self];
	[xmlReq setPriority:XMLREQ_PRIO_HIGH];
	[xmlReq performAsyncRequest];
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Set flag */
	[self setRegoInProgress:NO];
	
	/* Check success */
	if ([sender success])
	{
		/* OK */
		[self setProgressString:@"License key successfully uploaded."];		
		[tabView selectTabViewItemAtIndex:2];
	}
	else
	{
		/* Other error occurred */
		[self setProgressString:@"Failed to upload license key to Lithium Core."];
		[tabView selectTabViewItemAtIndex:1];
	}
	
	[customer highPriorityRefresh];
	
	/* Release request */
	[sender release];
}

#pragma mark "Accessors"

@synthesize customer;
@synthesize firstname;
@synthesize lastname;
@synthesize company;
@synthesize email;
@synthesize licenseIndex;
@synthesize regoInProgress;
@synthesize regoMessage;
@synthesize licenseKey;
@synthesize progressString;

@end
