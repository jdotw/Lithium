//
//  MBScriptUploadRequest.m
//  ModuleBuilder
//
//  Created by James Wilson on 23/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBModuleUploadRequest.h"


@implementation MBModuleUploadRequest

#pragma mark "Constructors"

+ (MBModuleUploadRequest *) uploadModule:(NSData *)initModuleData named:(NSString *)initName toCore:(MBCoreDeployment *)initCore
{
	return [[[MBModuleUploadRequest alloc] initWithData:initModuleData named:initName forCore:initCore] autorelease];
}

- (MBModuleUploadRequest *) initWithData:(NSData *)initModuleData named:(NSString *)initName forCore:(MBCoreDeployment *)initCore
{
	[self init];
	
	core = [initCore retain];
	moduleData = [initModuleData copy];
	name = [initName copy];
	
	return self;
}

- (MBModuleUploadRequest *) init
{
	[super init];
	
	return self;
}

- (void) dealloc 
{
	if (core) [core release];
	if (moduleData) [moduleData release];
	if (name) [name release];
	if (urlConn) { [urlConn cancel]; [urlConn release]; }
	if (receivedData) [receivedData release];
	[super dealloc];
}

#pragma mark "Request Manipulation"

- (void) performAsyncRequest
{
	NSString *url_str;
	NSMutableData *post_data;
	
	/* Init */
	success = NO;
	
	/* Create URL String */
	url_str = [NSString stringWithFormat:@"http://%@:51180/%@/script.php?action=upload&resaddr=%@&entaddr=%@&type=modb&name=%@.xml", 
			   core.ipAddress, core.selectedCustomer.name, [core.selectedCustomer resourceAddressString], [core.selectedCustomer entityAddressString], name];
	
	/* Setup URL request */
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url_str]
														   cachePolicy:NSURLRequestReloadIgnoringCacheData
													   timeoutInterval:30.0];
	/* Outbound XML doc to be sent */
	NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
	NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
	[url_req addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
	[url_req setHTTPMethod: @"POST"];
	post_data = [NSMutableData data];
	[post_data appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[post_data appendData:[@"Content-Disposition: form-data; name=\"scriptfile\"; filename=\"scriptfile\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[post_data appendData:[@"Content-Type: text/plain\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[post_data appendData:moduleData];
	[post_data appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[url_req setHTTPBody:post_data];
	
	/* Perform request */
	urlConn = [[NSURLConnection connectionWithRequest:url_req delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
		[self setStatus:@"Proceeding"];
	}
	else
	{ 
		/* Connection failed */
		[self setStatus:@"Failed"];
		[self connection:nil didFailWithError:nil];
	}	
	
	[self setInProgress:YES];
	[self setProgressString:@"Uploading and processing script..."];
	
	return;
}

- (void) cancel
{
	/* Cancels the HTTP request in progress */
	
	/* Remove activity */
	[self setStatus:@"Cancelled"];
	
	/* Clear instance variables */
	[self setDelegate:nil];
	[self setInProgress:NO];
	
	/* Cancel and free the NSURLConnection */
	if (urlConn) 
	{ 
		[urlConn cancel];
		[urlConn release];
		urlConn = nil;
	}
}

#pragma mark HTTP Handling

- (void) connection:(NSURLConnection *)connection 
 didReceiveResponse:(NSURLResponse *)response
{
	/* Received non-data response */
	[receivedData setLength:0];
	[self setStatus:@"Received non-data response"];
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
	[self setStatus:@"Receiving data"];
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			 willSendRequest:(NSURLRequest *)request
			redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
	[self setStatus:@"Received re-direction"];
    return request;
}

-(void) connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	[self setStatus:@"Received authentication challenge"];
	
	/* Check to see if this is the first attempt */
	if ([challenge previousFailureCount] == 0)
	{
		/* First attempt */
		NSURLCredential *cred = [NSURLCredential credentialWithUser:core.username
														   password:core.password
														persistence:NSURLCredentialPersistenceForSession];
		[[challenge sender] useCredential:cred forAuthenticationChallenge:challenge];
	}
	else
	{
		/* Do not attempt retry */
		[[challenge sender] cancelAuthenticationChallenge:challenge];
		[self setStatus:@"Authentication failed"];
	}
}		

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Download finished */
	
	/* Set success */
	success = YES;
	
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
	
	/* Call finished func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"moduleUploadFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set activity */
	[self setStatus:@"Request Finished"];
	[self setInProgress:NO];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Report status */
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Set success */
	success = NO;
	
	/* Set status */
	[self setProgressString:[NSString stringWithFormat:@"Failed to upload script (%@).", error]]; 
	[self setProgressIcon:[NSImage imageNamed:@"stop_48.tif"]];
	
	/* Call finished func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"moduleUploadFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set activity */
	[self setStatus:@"Request Failed"];
	[self setInProgress:NO];
}

- (void) disabledRefreshTimerCallback
{
	/* Called by an immediate timer callback when a queued 
	 * request is to be processed for a disabled customer
	 */
	
	/* Set success */
	success = NO;
	
	/* Call finished func */
	if (delegate)
	{
		SEL finishedsel = NSSelectorFromString(@"moduleUploadFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}		
	
	/* Set activity */
	[self setStatus:@"Request Disabled"];
	[self setInProgress:NO];
	return;
}

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
	/* Check for message */
	if ([element isEqualToString:@"message"])
	{ 
		[self setProgressString:curXMLString]; 
		[self setProgressIcon:[NSImage imageNamed:@"ok_48.tif"]];
	}
	
	/* Check for errror */
	if ([element isEqualToString:@"error"])
	{ 
		[self setProgressString:curXMLString]; 
		[self setProgressIcon:[NSImage imageNamed:@"stop_48.tif"]];
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
}

#pragma mark "Properties"
@synthesize core;
@synthesize delegate;
@synthesize success;
@synthesize inProgress;
@synthesize progressString;
@synthesize progressIcon;
@synthesize status;

@end
