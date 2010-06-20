//
//  LCDiagWebServerName.m
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagWebServerName.h"


@implementation LCDiagWebServerName

- (void) performTest:(id)initDelegate
{
	[super performTest:initDelegate];

	
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"http://127.0.0.1:51180/diag/env.php"]
														   cachePolicy:NSURLRequestReloadIgnoringCacheData
													   timeoutInterval:10.0];
	urlConn = [[NSURLConnection connectionWithRequest:url_req delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{ 
		/* Connection failed */
		[self testFailed];
		return;
	}	
}

- (NSString *) testDescription
{
	return @"Check Client Service ServerName configuration";
}

#pragma mark HTTP Handling

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
	/* Download finished, parse the XML */
	NSXMLParser *parser;
	
	/* Validate */
	NSString *xml = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xml hasPrefix:@"<?xml"])
	{
		/* Create properties */
		serverProperties = [[NSMutableDictionary dictionary] retain];
		
		/* Parse XML doc */
		parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
		
		/* Check ServerName */
		if ([serverProperties objectForKey:@"SERVER_NAME"])
		{
			char *str = (char *) [[serverProperties objectForKey:@"SERVER_NAME"] cStringUsingEncoding:NSUTF8StringEncoding];
			if (strstr(str, ".local"))
			{ [self testFailed]; }
			else
			{ [self testPassed]; }
		}
		else
		{ [self testWarning]; }
	}
	else
	{
		[self testFailed];
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
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Failed */
	[self testFailed];
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
		[serverProperties setObject:xmlString forKey:xmlElement]; 
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


@end
