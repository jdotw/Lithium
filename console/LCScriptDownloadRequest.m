//
//  LCScriptDownloadRequest.m
//  Lithium Console
//
//  Created by James Wilson on 31/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCScriptDownloadRequest.h"


@implementation LCScriptDownloadRequest

+ (LCScriptDownloadRequest *) downloadScript:(NSString *)initScriptName type:(NSString *)initType forCustomer:(LCCustomer *)initCustomer to:(NSString *)initLocalFilename
{
	return [[[LCScriptDownloadRequest alloc] initWithFilename:initScriptName type:initType forCustomer:initCustomer to:initLocalFilename] autorelease];
}

- (LCScriptDownloadRequest *) initWithFilename:(NSString *)initScriptName type:(NSString *)initType forCustomer:(LCCustomer *)initCustomer  to:(NSString *)initLocalFilename
{
	[self init];
	
	customer = [initCustomer retain];
	scriptName = [initScriptName retain];
	localFilename = [initLocalFilename retain];
	type = [initType retain];
	activity = [[LCActivity activityWithDescription:[NSString stringWithFormat:@"Downloading %@ script %@", type, scriptName]
										forCustomer:customer
										   delegate:self
									   stopSelector:nil] retain];
	[activity setStatus:@"Allocated"];
	
	return self;
}

- (LCScriptDownloadRequest *) init
{
	[super init];
	
	return self;
}

- (void) dealloc 
{
	if (activity) [activity invalidate];
	if (customer) [customer release];
	if (scriptName) [scriptName release];
	if (localFilename) [localFilename release];
	if (type) [type release];
	if (urlConn) { [urlConn cancel]; [urlConn release]; }
	if (receivedData) [receivedData release];
	[super dealloc];
}

#pragma mark "Request Manipulation"

- (void) performAsyncRequest
{
	NSString *url_str;
	
	/* Init */
	success = NO;
	
	/* Check if customer is disabled */
	if ([customer disabled])
	{
		/* Add an immediate timer callback to fire which will
		* act as if the request was completed though unsuccessfully
		*/
		[[NSTimer scheduledTimerWithTimeInterval:0.0
										  target:self
										selector:@selector(disabledRefreshTimerCallback)
										userInfo:nil
										 repeats:NO] retain];
		return;
	}
	
	/* Create URL String */
	url_str = [NSString stringWithFormat:@"%@/script.php?action=download&resaddr=%@&entaddr=%@&type=%@&name=%@", 
		[customer url], [[customer resourceAddress] addressString], [[customer entityAddress] addressString], type, scriptName];
	
	/* Setup URL request */
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url_str]
														   cachePolicy:NSURLRequestReloadIgnoringCacheData
													   timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	
	/* Perform request */
	urlConn = [[NSURLConnection connectionWithRequest:url_req delegate:self] retain];
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
	
	[self setInProgress:YES];
	[self setProgressString:@"Downloading script..."];
	
	return;
}

- (void) cancel
{
	/* Cancels the HTTP request in progress */
	
	/* Remove activity */
	[activity setStatus:@"Cancelled"];
	[activity invalidate];
	[activity release];
	activity = nil;
	
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

-(void) connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	[activity setStatus:@"Received authentication challenge"];
	
	/* Check to see if the customer has been disabled */
	if ([customer disabled])
	{
		/* Cancel challenge */
		[[challenge sender] cancelAuthenticationChallenge:challenge];
		[activity setStatus:@"Authentication failed"];
		return;
	}
	
	/* Check to see if this is the first attempt */
	if ([challenge previousFailureCount] == 0)
	{
		/* First attempt */
		LCAuthenticator *auth = [LCAuthenticator authForCustomer:customer];
		NSURLCredential *cred;
		cred = [NSURLCredential credentialWithUser:[auth username]
										  password:[auth password]
									   persistence:NSURLCredentialPersistenceForSession];
		[[challenge sender] useCredential:cred
			   forAuthenticationChallenge:challenge];
	}
	else
	{
		/* Subsequent attempts */
		LCAuthenticator *auth = [LCAuthenticator authForCustomer:customer];
		BOOL retry = [auth retryAuth];
		
		if (retry == YES)
		{
			/* Attempt retry */
			NSURLCredential *cred;
			cred = [NSURLCredential credentialWithUser:[auth username]
											  password:[auth password]
										   persistence:NSURLCredentialPersistenceForSession];
			[[challenge sender] useCredential:cred forAuthenticationChallenge:challenge];
		}
		else
		{
			/* Do not attempt retry */
			[[challenge sender] cancelAuthenticationChallenge:challenge];
			[activity setStatus:@"Authentication failed"];
		}
	}
	
}		

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Download finished */
	
	/* Set success */
	success = YES;
	
	/* Write to temp location */
	[receivedData writeToFile:localFilename atomically:YES];
	
	/* Call finished func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"scriptDownloadFinished:");
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
	[activity setStatus:@"Request Finished"];
	[activity invalidate];
	[activity release];
	activity = nil;
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
	
	/* Call finished func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"scriptDownloadFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set activity */
	[activity setStatus:@"Request Failed"];
	[activity invalidate];
	[activity release];
	activity = nil;
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
		SEL finishedsel = NSSelectorFromString(@"scriptDownloadFinished:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}		
	
	/* Set activity */
	[activity setStatus:@"Request Disabled"];
	[activity invalidate];
	[activity release];
	activity = nil;
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
	
	/* Check for scriptOutput */
	if ([element isEqualToString:@"script_output"])
	{ [self setScriptOutput:curXMLString]; }
	
	/* Check for error */
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
}

#pragma mark "Accessor Methods"

- (id) customer						{ return customer; }
- (void) setCustomer:(id)custent	{ customer = custent; }

- (id) delegate					{ return delegate; }
- (void) setDelegate:(id)obj	
{ 
	delegate = obj; 
}

- (BOOL) success				{ return success; }
- (void) setSuccess:(BOOL)flag	{ success = flag; }

- (NSMutableData *) receivedData				{ return receivedData; }
- (void) setReceivedData:(NSMutableData *)data	{ if (receivedData) [receivedData release]; receivedData = [data retain]; }

- (BOOL) inProgress
{ return inProgress; }
- (void) setInProgress:(BOOL)flag
{ inProgress = flag; }

- (NSString *) progressString
{ return progressString; }

- (void) setProgressString:(NSString *)string
{
	[progressString release];
	progressString = [string retain];
}

- (NSString *) scriptOutput
{ return scriptOutput; }

- (void) setScriptOutput:(NSString *)string
{ 
	[scriptOutput release];
	scriptOutput = [string retain];
}

- (NSImage *) progressIcon
{ return progressIcon; }

- (void) setProgressIcon:(NSImage *)icon
{
	[progressIcon release];
	progressIcon = [icon retain];
}


@synthesize type;
@synthesize scriptName;
@synthesize localFilename;
@synthesize urlConn;
@synthesize activity;
@synthesize curXMLElement;
@synthesize curXMLString;
@end
