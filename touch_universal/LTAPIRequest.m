//
//  LTAPIRequest.m
//  Lithium
//
//  Created by James Wilson on 22/12/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTAPIRequest.h"

#import "AppDelegate.h"
#import "LTAuthenticationTableViewController.h"
#import "LTCoreDeployment.h"

@implementation LTAPIRequest

- (void) main
{
	/* The subclass will create a urlRequest for us to use, then call [super main] */
	
	/* Execute the API Request */
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	[appDelegate performSelectorOnMainThread:@selector(apiCallDidBegin:)
								  withObject:self
							   waitUntilDone:NO
									   modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, UITrackingRunLoopMode, nil]];

	if ([delegate respondsToSelector:@selector(apiCallDidBegin:)])
	{
		[delegate performSelectorOnMainThread:@selector(apiCallDidBegin:)
									  withObject:self
								   waitUntilDone:NO
										   modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, UITrackingRunLoopMode, nil]];
	}		
	
	/* Create import pool */
//	NSAutoreleasePool *importPool = [[NSAutoreleasePool alloc] init];

	/* Execute the subclass-created request */
	finished = NO;
	if (debug) NSLog (@"INFO: %@ - Fetching %@", self, [urlReq URL]);
	NSURLConnection *urlConn = [NSURLConnection connectionWithRequest:urlReq delegate:self];
	if (urlConn != nil)
	{
		/* Run main run-loop until connection loads */
		while (!finished)
		{ [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]]; }
	}
	
	/* Signal completion to delegates */
	if (delegate && [delegate respondsToSelector:@selector(apiCallDidFinish:)])
	{
		[delegate performSelectorOnMainThread:@selector(apiCallDidFinish:)
								   withObject:self
								waitUntilDone:NO
										modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, UITrackingRunLoopMode, nil]];
	}
	

	/* Notify the app delegate that we're done */
	[appDelegate performSelectorOnMainThread:@selector(apiCallDidFinish:)
								  withObject:self
							   waitUntilDone:NO
									   modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, UITrackingRunLoopMode, nil]];
	
	/* Clean up */
//	[importPool drain];
	
}

- (void) dealloc
{
	[super dealloc];
}

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *) space 
{
	if([[space authenticationMethod] isEqualToString:NSURLAuthenticationMethodServerTrust]) 
	{
		return YES; // Self-signed cert will be accepted
	}
	else
	{ 
		return YES;
	}
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    [receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    [receivedData appendData:data];
}

-(void) connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *) challenge
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	LTAuthenticationTableViewController *authViewController = appDelegate.authViewController;
	[authViewController requestAuthForCustomer:self.customer
										entity:self.customer
									 challenge:challenge
									connection:connection
						  previousFailureCount:[challenge previousFailureCount]];
}		

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{	
    /* Set State */
	refreshInProgress = NO;
	finished = YES;
	
	/* FIX */
    NSLog(@"ERROR: Connection failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSURLErrorFailingURLStringErrorKey]);	
    
    NSLog (@"FAIL for %@ (%@)", self.customer.name, self.customer.coreDeployment);

    /* Record error */
    LTCoreDeployment *coreDeployment = nil;
    if (self.customer) coreDeployment = self.customer.coreDeployment;
    else if ([self isMemberOfClass:[LTCoreDeployment class]]) coreDeployment = (LTCoreDeployment *)self;
    if (coreDeployment && !coreDeployment.lastRefreshFailed)
    {
        /* First (recent) failure, post a reachability change notification */
        [[NSNotificationCenter defaultCenter] postNotificationName:@"LTCoreDeploymentReachabilityChanged" 
                                                            object:coreDeployment];
    }
    coreDeployment.lastRefreshFailed = YES;
    [[NSNotificationCenter defaultCenter] postNotificationName:kLTAPIRequestFailed object:self];
    
    /* Clean up */
//    [connection release]; // DO NOT RELEASE -- In the NSOperation mode it's held onto by main
    [receivedData release];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    /* Set State */
	refreshInProgress = NO;
	finished = YES;

    /* Cleanup */
    [receivedData release];
//	[connection release]; // DO NOT RELEASE -- In the NSOperation mode it's held onto by main
    
    /* Reset failure stats */
    NSLog (@"SUCCESS for %@ (%@)", self.customer.name, self.customer.coreDeployment);
    self.customer.coreDeployment.lastRefreshFailed = NO;
    self.customer.coreDeployment.refreshFailAlertShown = NO;
}

@synthesize customer;
@synthesize refreshInProgress;
@synthesize delegate;
@synthesize debug;

@end
