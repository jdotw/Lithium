//
//  LCXMLRequest.m
//  Lithium Console
//
//  Created by James Wilson on 15/08/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXMLRequest.h"
#import "LCXMLRequestQueue.h"
#import "LCError.h"
#import "LCXMLParseOperation.h"
#import "LCConsoleController.h"

@implementation LCXMLRequest

/* Initialisation */

+ (LCXMLRequest *) requestWithCriteria:(id)initcust 
							  resource:(LCResourceAddress *)initres 
								entity:(LCEntityAddress *)initent 
							   xmlname:(NSString *)initxmlname 
								refsec:(time_t)initrefsec 
								xmlout:(NSXMLDocument *)initxmlout
{
	return [[[LCXMLRequest alloc] initWithCriteria:initcust 
								   resource:initres 
									 entity:initent 
									xmlname:initxmlname 
									 refsec:initrefsec
									 xmlout:initxmlout] autorelease];
}

- (LCXMLRequest *) initWithCriteria:(id)initcust 
						   resource:(LCResourceAddress *)initres 
							 entity:(LCEntityAddress *)initent 
							xmlname:(NSString *)initxmlname 
							 refsec:(time_t)initrefsec 
							 xmlout:(NSXMLDocument *)initxmlout
{
	[self init];
	
	cust = [initcust retain];
	resource = [initres retain];
	if (initent) entity = [initent retain];
	xmlname = [initxmlname retain];
	refsec = initrefsec;
	if (initxmlout) xmlout = [initxmlout retain];

	activity = [[LCActivity activityWithDescription:[self requestDescription]
									   forCustomer:[self customer]
										  delegate:self
									  stopSelector:nil] retain];
	[activity setStatus:@"Allocated"];

	return self;
}

- (LCXMLRequest *) init
{
	[super init];
	
	return self;
}

- (void) dealloc 
{
	if (activity) [activity invalidate];
	if (cust) [cust release];
	if (resource) [resource release];
	if (entity) [entity release];
	if (xmlname) [xmlname release];
	if (xmlout) [xmlout release];
	if (urlConn) { [urlConn cancel]; [urlConn release]; }
	if (receivedData) [receivedData release];
	if (lastUsername) [lastUsername release];
	if (lastPassword) [lastPassword release]; 
	if (xmlParser)
	{
		/* Parser operation was pending.. it can't 
		 * be removed from the queue so just ensure there
		 * is no delegate and that we're not listening 
		 */
		[xmlParser removeObserver:self forKeyPath:@"isFinished"];
		xmlParser.delegate = nil;
	}
	[super dealloc];
}

#pragma mark "Request Manipulation"

- (void) performAsyncRequest
{
	/* Called when a request is ready to be performed.
	 * The request is enqueued to the master LCXMLRequestQueue queue
	 */

	/* Perform real request */
	[[LCXMLRequestQueue masterQueue] enqueueRequest:self];
	[activity setStatus:@"Queued"];
	[self setInProgress:YES];
}

- (void) performQueuedRequest
{
	/* Called by the LCXMLRequestQueue when the request
	 * is to be performed according to the queuing mechanism
	 */
	NSString *url_str;
	NSMutableData *post_data;
	
	/* Init */
	success = NO;
	
	/* Check if customer is disabled */
	if ([cust disabled])
	{
		/* Add an immediate timer callback to fire which will
		 * act as if the request was completed though unsuccessfully
		 */
		[NSTimer scheduledTimerWithTimeInterval:0.0
										 target:self
									   selector:@selector(disabledRefreshTimerCallback)
									   userInfo:nil
										 repeats:NO];
		return;
	}
	
	/* Create URL String */
	if (entity)
	{
		url_str = [NSString stringWithFormat:@"%@/xml.php?action=xml_get&resaddr=%@&entaddr=%@&xmlname=%@&refsec=%li", 
			[cust url], [resource addressString], [entity addressString], xmlname, refsec];
	}
	else
	{
		url_str = [NSString stringWithFormat:@"%@/xml.php?action=xml_get&resaddr=%@&xmlname=%@&refsec=%li", 
			[cust url], [resource addressString], xmlname, refsec];
	}
	if (debug)
	{ NSLog (@"[LCXMLRequest performQueuedRequest] url is %@", url_str); }
	
	/* Setup URL request */
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url_str]
														   cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
													   timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	if (xmlout)
	{
		/* Outbound XML doc to be sent */
		NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
		NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
		[url_req addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
		[url_req setHTTPMethod: @"POST"];
		post_data = [NSMutableData data];
		[post_data appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
		[post_data appendData:[@"Content-Disposition: form-data; name=\"xmlfile\"; filename=\"lconsole.xml\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
		[post_data appendData:[@"Content-Type: text/xml\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];

		/* CRLF Hack */
		NSMutableString *xmlString = [NSMutableString stringWithString:[xmlout XMLString]];
		[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
		[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
		/* END CRLF Hack */

		if (debug)
		{ NSLog (@"[LCXMLRequest performQueuedRequest] outbound XML is %@", xmlString); }
		
		[post_data appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
		[post_data appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
		[url_req setHTTPBody:post_data];
	}
	
	/* Perform request */
	urlConn = [[NSURLConnection connectionWithRequest:url_req delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
		if (debug) NSLog (@"[LCXMLRequest performQueuedRequest] request proceeding");
		[activity setStatus:@"Proceeding"];
	}
	else
	{ 
		/* Connection failed */
		if (debug) NSLog (@"[LCXMLRequest performQueuedRequest] connection failed");
		[activity setStatus:@"Failed"];
		[self connection:nil didFailWithError:nil];
	}	
	
	return;
}

- (void) cancel
{
	/* Cancels the HTTP request in progress */
	
	/* Remove from XML request queue */
	[[LCXMLRequestQueue masterQueue] dequeueRequest:self];

	/* Notify queue */
	if ([self inProgress] == YES)
	{ [[LCXMLRequestQueue masterQueue] queuedRequestFinished:self]; }
	
	/* Remove activity */
	[activity setStatus:@"Cancelled"];
	[activity invalidate];
	[activity release];
	activity = nil;

	/* Clear instance variables */
	[self setDelegate:nil];
	[self setXMLDelegate:nil];
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
	if (debug) NSLog (@"[LCXMLRequest connection:didReceiveResponse");
	[activity setStatus:@"Received non-data response"];
}

-(void)	connection:(NSURLConnection *) connection
	didReceiveData:(NSData *) data
{
	/* Received the actual data */
	[receivedData appendData:data];
	if (debug) NSLog (@"[LCXMLRequest connection:didReceiveData");
	[activity setStatus:@"Receiving data"];
}

-(NSURLRequest *)	connection:(NSURLConnection *)connection
			   willSendRequest:(NSURLRequest *)request
			  redirectResponse:(NSURLResponse *)redirectResponse
{
	/* Allow redirects */
	if (debug) NSLog (@"[LCXMLRequest connection:willSendRequest:redirectResponse] redirected to %@", request);
	[activity setStatus:@"Received re-direction"];
    return request;
}

-(void) connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	if (debug) NSLog (@"[LCXMLRequest connection:didReceiveAuthenticationChallenge");

	[activity setStatus:@"Received authentication challenge"];
	
	/* Check to see if the customer has been disabled */
	if ([cust disabled])
	{
		/* Cancel challenge */
		[[challenge sender] cancelAuthenticationChallenge:challenge];
		[activity setStatus:@"Customer disabled"];
		return;
	}
	
	/* Create authenticator */
	LCAuthenticator *auth = [LCAuthenticator authForCustomer:cust];
	BOOL authHasChanged;
	if (![[auth username] isEqualToString:lastUsername] || ![[auth password] isEqualToString:lastPassword])
	{ authHasChanged = YES; }
	else
	{ authHasChanged = NO; }
	
	/* Check to see if this is the first attempt 
     * Or if the authentication credentials used 
	 * last time have changed
	 */
	
	if ([cust disabled])
	{
		[[challenge sender] cancelAuthenticationChallenge:challenge];
		return;
	}
	
	if ([challenge previousFailureCount] == 0 || authHasChanged == YES)
	{
		NSURLCredential *cred;
		cred = [NSURLCredential credentialWithUser:[auth username]
										  password:[auth password]
									   persistence:NSURLCredentialPersistenceForSession];
		[[challenge sender] useCredential:cred
			   forAuthenticationChallenge:challenge];
		
		/* Set last-used variables */
		[self setLastUsername:[auth username]];
		[self setLastPassword:[auth password]];
	}
	else
	{
		/* Subsequent attempt, and there has been
		 * no change in auth credentials for this
		 * customer
		 */
		BOOL retry = [auth retryAuth];
		
		if (retry == YES)
		{
			/* Attempt retry */
			NSURLCredential *cred;
			cred = [NSURLCredential credentialWithUser:[auth username]
											  password:[auth password]
										   persistence:NSURLCredentialPersistenceForSession];
			[[challenge sender] useCredential:cred forAuthenticationChallenge:challenge];

			/* Set last-used variables */
			[self setLastUsername:[auth username]];
			[self setLastPassword:[auth password]];
		}
		else
		{
			/* Do not attempt retry */
			if (debug) NSLog (@"LCXMLRequest Authentication failure");
			[[challenge sender] cancelAuthenticationChallenge:challenge];
			[activity setStatus:@"Authentication failed"];
		}
	}
}		

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Download finished, parse the XML */

	/* Info */
	if (debug)
	{
		NSString *str = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
		NSLog (@"[LCXMLRequest connectionDidFinishLoading] recvd '%@'", str);
	}
	
	/* Notify queue */
	[[LCXMLRequestQueue masterQueue] queuedRequestFinished:self];
	
	/* DEBUG */
	if (debug)
	{ NSLog (@"Received %iKb for %@ %@", [receivedData length] / 1024, [self xmlname], [[self entity] addressString]); }
	
	/* Call pre-parse func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"XMLRequestPreParse:");
		if (finishedsel && [delegate respondsToSelector:finishedsel])
		{ [delegate performSelector:finishedsel withObject:self]; }
	}
		
	/* Validate */
	NSString *xmlString = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xmlString hasPrefix:[NSString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?><error>"]])
	{
		/* Error received */
		[LCError logError:[NSString stringWithFormat:@"Error XML Recieved for '%@'", [self xmlname]]
			  forCustomer:[self customer]
				 fullText:[NSString stringWithFormat:@"Server Returned: %@", xmlString]];

		/* Set success */
		success = NO;
		
		/* End of parsing */
		[self parserDidFinish];
	}
	else if ([xmlString hasPrefix:@"<?xml"])
	{
//		/* Cache if applicable */
//		if ([[self xmlname] isEqualToString:@"entity_tree"] && [[[self entity] type] intValue] == 1)
//		{
//			/* Entity tree for customer */
//			NSString *filename = [NSString stringWithFormat:@"%@/customer_%@_xml_tree.xml", 
//			[@"~/Library/Caches/com.lithiumcorp.Console" stringByExpandingTildeInPath], [[self entity] customerName]];
//			[receivedData writeToFile:filename atomically:NO];
//		}
//		else if ([[self xmlname] isEqualToString:@"entity_tree"] && [[[self entity] type] intValue] == 3)
//		{
//			/* Entity tree for customer */
//			NSString *filename = [NSString stringWithFormat:@"%@/device_%@_%@_%@_xml_tree.xml", 
//				[@"~/Library/Caches/com.lithiumcorp.Console" stringByExpandingTildeInPath], [[self entity] customerName], [[self entity] siteName], [[self entity] deviceName]];
//			[receivedData writeToFile:filename atomically:NO];
//		}

		/* Set success */
		success = YES;
		
		/* Check to see if we're working on an entity */
		if (threadedXmlDelegate)
		{
			/* For LCEntity XML parsing, create an NSOperation to perform the actual parsing 
			 * using the deleate provided. This will cause the LCEntity XML parsing
			 * to happen on a separate thread.
			 */		
			xmlParser = [LCXMLParseOperation new];
			xmlParser.xmlData = receivedData;
			xmlParser.delegate = threadedXmlDelegate;
			[xmlParser addObserver:self
						forKeyPath:@"isFinished" 
						   options:NSKeyValueObservingOptionNew 
						   context:nil];
			[[[LCConsoleController masterController] operationQueue] addOperation:xmlParser];
			[xmlParser autorelease];
		}
		else if (xmldelegate)
		{
			/* Otherwise, Parse immediately on current thread */
			NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
			[parser setDelegate:xmldelegate];
			[parser setShouldResolveExternalEntities:YES];
			[parser parse];
			[self parserDidFinish];
		}
		else 
		{
			/* No parser delegate */
			[self parserDidFinish];
		}

	}
	else
	{
		[LCError logError:[NSString stringWithFormat:@"Invalid XML Recieved for '%@'", [self xmlname]]
			  forCustomer:[self customer]
				 fullText:[NSString stringWithFormat:@"Server Returned: %@", xmlString]];
		
		/* Set success */
		success = NO;
		
		/* End of parsing */
		[self parserDidFinish];
	}	
}

- (void) parserDidFinish
{
	/* Check Log */
	if ([NSThread currentThread] != [NSThread mainThread])
	{
		[NSException raise:@"LCXMLRequest-parserDidFinish-IncorrectThread"
					format:@"An instance of LCXMLRequest received a message to parserDidFinish on a thread that was NOT that main thread"];
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
		
	/* Call the delegates finished func */
	if (delegate) 
	{
		if ([delegate respondsToSelector:@selector(XMLRequestFinished:)])
		{ [delegate performSelector:@selector(XMLRequestFinished:) withObject:self]; }
	}
	
	/* Release connection */
	[urlConn release];
	urlConn = nil;
	
	/* Set activity */
	[activity setStatus:@"Request Finished"];
	[activity invalidate];
	[activity release];
	activity = nil;
	[self setInProgress:NO];	
}

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"isFinished"])
	{
		/* Parse operation finished */
		[object removeObserver:self forKeyPath:@"isFinished"];
		if (object == xmlParser)
		{
			xmlParser = nil;
		}
		[self performSelectorOnMainThread:@selector(parserDidFinish)
							   withObject:nil
							waitUntilDone:NO];
	}		
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	/* Report status */
	NSLog (@"[LCXMLRequest didFailWithError] XML load failed for %@", xmlname);
	[LCError logError:[NSString stringWithFormat:@"Failed to retrieve XML %@", [self xmlname]]
		  forCustomer:[self customer]
			 fullText:[NSString stringWithFormat:@"Error: %@", [[error localizedDescription] capitalizedString]]];
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Set success */
	success = NO;

	/* Notify queue */
	[[LCXMLRequestQueue masterQueue] queuedRequestFinished:self];
	
	/* Call finished func */
	if (delegate) 
	{
		SEL finishedsel = NSSelectorFromString(@"XMLRequestFinished:");
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
	
	/* Notify queue */
	[[LCXMLRequestQueue masterQueue] queuedRequestFinished:self];
	
	/* Call finished func */
	if (delegate)
	{
		SEL finishedsel = NSSelectorFromString(@"XMLRequestFinished:");
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


#pragma mark "Accessor Methods"

- (id) customer						{ return cust; }
- (void) setCustomer:(id)custent	{ cust = custent; }

- (LCResourceAddress *) resource					{ return resource; }
- (void) setResource:(LCResourceAddress *)resaddr	{ if (resource) [resource release]; resource = [resaddr retain]; }

- (LCEntityAddress *) entity					{ return entity; }
- (void) setEntity:(LCEntityAddress *)entaddr	{ if (entity) [entity release]; entity = [entaddr retain]; }

- (NSString *) xmlname					{ return xmlname; }
- (void) setXMLname:(NSString *) name	{ if (xmlname) [xmlname release]; xmlname = [name retain]; }

- (NSTimeInterval) refsec					{ return refsec; }
- (void) setRefsec:(NSTimeInterval)time		{ refsec = time; }

- (NSXMLDocument *) xmlout				{ return xmlout; }
- (void) setXMLout:(NSXMLDocument *)doc	{ if (xmlout) [xmlout release]; xmlout = [doc retain]; }

- (id) delegate					{ return delegate; }
- (void) setDelegate:(id)obj	
{ 
	delegate = obj; 
	[activity setDescription:[self requestDescription]];
}

- (NSString *) requestDescription
{ 
	NSString *str;
	
	if (entity)
	{ str = [NSString stringWithFormat:@"Requesting XML %@ from entity %@ at resource %@ (delegate %@)", 
		xmlname, [entity addressString], [resource addressString], delegate]; }
	else
	{ str = [NSString stringWithFormat:@"Requesting XML %@ from resource %@ (delegate %@)", 
		xmlname, [resource addressString], delegate]; }
	
	return str;
}

- (id) xmlDelegate				{ return delegate; }
- (void) setXMLDelegate:(id)obj	{ xmldelegate = obj; }

- (BOOL) success				{ return success; }
- (void) setSuccess:(BOOL)flag	{ success = flag; }

- (BOOL) debug					{ return debug; }
- (void) setDebug:(BOOL)flag	{ debug = flag; }

- (NSMutableData *) receivedData				{ return receivedData; }
- (void) setReceivedData:(NSMutableData *)data	{ if (receivedData) [receivedData release]; receivedData = [data retain]; }

- (int) priority
{ return priority; }
- (void) setPriority:(int)newPriority
{ 
	/* Check if request is in progress */
	if (inProgress == YES)
	{
		/* Connection in progress, can't change priority */
		return; 
	}
	
	/* Set priority and update activity */
	priority = newPriority;
	[activity setPriority:[NSNumber numberWithInt:priority]];
	
	/* Call a queue run */
	[[LCXMLRequestQueue masterQueue] runQueues];
}

- (BOOL) inProgress
{ return inProgress; }
- (void) setInProgress:(BOOL)flag
{ inProgress = flag; }

- (NSString *) lastUsername
{ return lastUsername; }

- (void) setLastUsername:(NSString *)string
{
	[lastUsername release];
	lastUsername = [string retain];
}

- (NSString *) lastPassword
{ return lastPassword; }

- (void) setLastPassword:(NSString *)string
{
	[lastPassword release];
	lastPassword = [string retain];
}

@synthesize xmlname;
@synthesize xmlout;
@synthesize xmldelegate;
@synthesize timeout;
@synthesize urlConn;
@synthesize activity;

@synthesize threadedXmlDelegate;

@end
