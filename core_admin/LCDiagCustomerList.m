//
//  LCDiagCustomerList.m
//  LCAdminTools
//
//  Created by James Wilson on 4/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCustomerList.h"

#import "LCDiagCustomerWebPerms.h"
#import "LCDiagCustomerAuth.h"

@implementation LCDiagCustomerList

- (void) performTest:(id)initDelegate
{
	[super performTest:initDelegate];
	
	while ([childTests count] > 0)
	{ [self removeObjectFromChildTestsAtIndex:0]; }
	
	customers = [[NSMutableArray array] retain];
	
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"http://127.0.0.1:51180/console.php"]
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
	return @"Check Customer List (console.php)";
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
		/* Parse XML doc */
		parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		[parser parse];
		
		/* Add child tests */
		NSEnumerator *customerEnum = [customers objectEnumerator];
		NSMutableDictionary *custProperties;
		while (custProperties = [customerEnum nextObject])
		{
			[self insertObject:[[LCDiagCustomerWebPerms alloc] initForCustomerName:[custProperties objectForKey:@"name"]] inChildTestsAtIndex:[childTests count]];
			[self insertObject:[[LCDiagCustomerAuth alloc] initForCustomerName:[custProperties objectForKey:@"name"]] inChildTestsAtIndex:[childTests count]];
		}

		/* CHeck count */
		if ([customers count] > 0)
		{ [self testPassed]; }
		else
		{ [self testFailed]; }
		
		/* Clean up */
		[customers release];
		customers = nil;
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
	
	/* Check for start of a customer */
	if ([element isEqualToString:@"customer"])
	{
		curProperties = [[NSMutableDictionary dictionary] retain];
	}
	
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
		[curProperties setObject:xmlString forKey:xmlElement]; 
		[xmlString release];
		xmlString = nil;
	}
	
	/* Check for the end of a customer */
	if ([element isEqualToString:@"customer"])
	{
		[customers addObject:curProperties];
		[curProperties release];
		curProperties = nil;
	}
	
	/* Release current element */
	if (xmlElement)
	{
		[xmlElement release];
		xmlElement = nil;
	}
}

@end
