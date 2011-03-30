//
//  LCDemoLicense.m
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LCDemoLicense.h"
#import "LCCustomer.h"

@implementation LCDemoLicense

@synthesize name, email, company, signedKey, delegate;

#pragma mark Public Method

- (void) requestLicenseForCustomer:(LCCustomer *)customer
{
	/* Create URL String */
	NSURL *baseURL = [NSURL URLWithString:[customer url]];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/register_demo5.php?firstname=%@&lastname=&company=%@&email=%@&licindex=0&custname=%@&host=%@", 
						   [self.name stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [self.company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [self.email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding], 
						   [[customer name] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [[baseURL host] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	
	/* Setup URL request */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	/* Perform request */
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{ 
		/* Connection failed */
		[self connection:nil didFailWithError:nil];
	}	
}

#pragma mark Memory Management

- (void) dealloc
{
	[name release];
	[email release];
	[company release];
	[super dealloc];
}

#pragma mark HTTP Delegate

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[receivedData setLength:0];
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			 willSendRequest:(NSURLRequest *)request
			redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
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

		/* Inform delegate */
		[delegate demoLicenseGranted:self];
	}	
	else
	{
		/* Inform delegate of failure (junk received) */
		[delegate demoLicenseNotGranted:self error:nil];
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	if (connection == urlConn)
	{
		[urlConn release];
		urlConn = nil;
	}
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Report status */
	[delegate demoLicenseNotGranted:self error:error];
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	if (connection == urlConn)
	{
		[urlConn release];
		urlConn = nil;
	}
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
	/* Check for key */
	if ([element isEqualToString:@"key"])
	{ 
		self.signedKey = curXMLString;
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
}

@end
