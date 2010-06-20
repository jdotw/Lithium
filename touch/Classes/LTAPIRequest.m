//
//  LTAPIRequest.m
//  Lithium
//
//  Created by James Wilson on 22/12/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTAPIRequest.h"

#import "LithiumAppDelegate.h"
#import "LTAuthenticationTableViewController.h"

@implementation LTAPIRequest

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
	LithiumAppDelegate *appDelegate = (LithiumAppDelegate *) [[UIApplication sharedApplication] delegate];
	LTAuthenticationTableViewController *authViewController = appDelegate.authViewController;
	[authViewController requestAuthForCustomer:self.customer
										entity:self.customer
									 challenge:challenge
									connection:connection
						  previousFailureCount:[challenge previousFailureCount]];
}		

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    [connection release];
    [receivedData release];
	
	refreshInProgress = NO;
	
	/* FIX */
    NSLog(@"ERROR: Connection failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSErrorFailingURLStringKey]);	
}

@synthesize customer;
@synthesize refreshInProgress;

@end
