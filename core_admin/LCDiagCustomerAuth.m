//
//  LCDiagCustomerAuth.m
//  LCAdminTools
//
//  Created by James Wilson on 4/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCDiagCustomerAuth.h"
#import "LCDiagCustomerResourceRestarts.h"
#import "LCDiagCustomerResourceHeartbeats.h"

#import "LCConfigController.h"

@implementation LCDiagCustomerAuth

- (LCDiagCustomerAuth *) initForCustomerName:(NSString *)name
{
	[super init];
	customerName = [name retain];
	[self insertObject:[[LCDiagCustomerResourceRestarts alloc] initForCustomerName:customerName] inChildTestsAtIndex:[childTests count]];
	[self insertObject:[[LCDiagCustomerResourceHeartbeats alloc] initForCustomerName:customerName] inChildTestsAtIndex:[childTests count]];
	return self;
}

- (void) performTest:(id)initDelegate
{
	[super performTest:initDelegate];
	
	NSString *urlString = [NSString stringWithFormat:@"http://127.0.0.1:51180/%@/diag_auth.php", customerName];
	NSMutableURLRequest *url_req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
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
	return [NSString stringWithFormat:@"Attempting to log in to the '%@' customer", customerName];
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

-(void) connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	NSURLCredential *cred;
	cred = [NSURLCredential credentialWithUser:[[LCConfigController masterController] adminUsername]
									  password:[[LCConfigController masterController] adminPassword]
								   persistence:NSURLCredentialPersistenceForSession];
	[[challenge sender] useCredential:cred
		   forAuthenticationChallenge:challenge];	
}

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Validate */
	NSString *result = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([result isEqualToString:@"OK"])
	{ [self testPassed]; }
	else
	{ [self testFailed]; }
	
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


@end
