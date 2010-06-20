//
//  LTPushRegistrationRequest.m
//  Lithium
//
//  Created by James Wilson on 21/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTPushRegistrationRequest.h"

#import "NSData-Base64.h"

@implementation LTPushRegistrationRequest

- (id) initWithCustomer:(LTCustomer *)initCustomer token:(NSData *)initToken receiveNotifications:(BOOL)initFlag
{
	[super init];
	
	/* Setup properties */
	self.customer = initCustomer;
	self.token = initToken;
	self.receiveNotifications = initFlag;
	
	return self;
}

- (void) dealloc
{
	[customer release];
	[super dealloc];
}

#pragma mark "Request Methods"

- (void) performRequest
{	
	NSMutableString *tokenString = [NSMutableString stringWithString:[self.token base64Encoding]];
	[tokenString replaceOccurrencesOfString:@"+" withString:@"%2b" options:0 range:NSMakeRange(0, [tokenString length])];
	[tokenString replaceOccurrencesOfString:@"/" withString:@"%2f" options:0 range:NSMakeRange(0, [tokenString length])];
	NSString *phpFile;
	if (receiveNotifications) phpFile = @"register.php";
	else phpFile = @"deregister.php";
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/pud/%@?uuid=%@&token=%@", 
						   phpFile,
						   [customer.uuidString stringByAddingPercentEscapesUsingEncoding:NSASCIIStringEncoding],
						   tokenString];
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];

	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		receivedData=[[NSMutableData data] retain];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to contact Lithium Push Dispatched");
	}	
	
}

- (void) performReset
{	
	NSMutableString *tokenString = [NSMutableString stringWithString:[self.token base64Encoding]];
	[tokenString replaceOccurrencesOfString:@"+" withString:@"%2b" options:0 range:NSMakeRange(0, [tokenString length])];
	[tokenString replaceOccurrencesOfString:@"/" withString:@"%2f" options:0 range:NSMakeRange(0, [tokenString length])];
	NSString *urlString = [NSString stringWithFormat:@"https://secure.lithiumcorp.com.au/pud/reset.php?token=%@", tokenString];
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
	
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		receivedData=[[NSMutableData data] retain];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to contact Lithium Push Dispatched");
	}	
	
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    [connection release];
    [receivedData release];
	[self autorelease];
}

#pragma mark "Properties"

@synthesize token;
@synthesize receiveNotifications;

@end
