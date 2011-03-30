//
//  LCLicense.m
//  Lithium Console
//
//  Created by James Wilson on 29/03/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LCLicense.h"
#import "LCCustomer.h"

@implementation LCLicense

@synthesize name, company, email, key, signedKey, delegate;

#pragma mark Public Methods

- (void) activateLicenseForCustomer:(LCCustomer *)customer
{
	/* Create URL String */
	NSURL *baseURL = [NSURL URLWithString:customer.url];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/vince/activate_license5.php?firstname=%@&lastname=&company=%@&email=%@&custname=%@&host=%@&key=%@", 
						   [self.name stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [self.company stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [self.email stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [customer.name stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [[baseURL host] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
						   [self.key stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
	
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

- (BOOL) requiresActivation
{
	if ([self.key length] > 30 && [self.key length] < 50)
	{
		/* eKey requiring activation */
		return YES;
	}
	else
	{
		return NO;
	}	
}

- (BOOL) isValid
{
	if ([self.key length] > 30 && [self.key length] < 50)
	{
		/* eKey requiring activation */
		return YES;
	}
	else if ([self.key length] >= 50)
	{
		/* Signed key does not need activation */
		return YES;
	}
	else 
	{
		/* Invalid size */
		return NO;
	}
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
	NSString *xml = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xml hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];

		/* Inform Delegate */
		[delegate licenseActivated:self];
	}
	else
	{
		/* Inform delegate of failure (junk received) */
		[delegate licenseNotActivated:self error:nil];
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
	[delegate licenseNotActivated:self error:error];
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
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
		if ([element isEqualToString:@"key"])
		{ 
			self.signedKey = xmlString;
		}
	}
	
	/* Release current element */
	[xmlString release];
	xmlString = nil;
}


@end
