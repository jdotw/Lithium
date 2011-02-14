//
//  LTCoreDeployment.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <sys/socket.h>
#import <netinet/in.h>
#import <netinet6/in6.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#include <netdb.h>

#import "LTCoreDeployment.h"
#import "AppDelegate.h"
#import "LCXMLNode.h"
#import "TBXML-Lithium.h"


@implementation LTCoreDeployment



#pragma mark "Constructors"

- (LTCoreDeployment *) init
{
	[super init];
	
	self.coreDeployment = self;
	
	return self;
}

- (LTCoreDeployment *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	self.ipAddress = [decoder decodeObjectForKey:@"ipAddress"];
	self.desc = [decoder decodeObjectForKey:@"desc"];
	self.enabled = [decoder decodeBoolForKey:@"enabled"];
	self.useSSL = [decoder decodeBoolForKey:@"useSSL"];
	self.uuidString = [decoder decodeObjectForKey:@"uuidString"];
	self.name = self.ipAddress;
	if ([self.desc length] < 1)
	{ self.desc = self.ipAddress; }
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:ipAddress forKey:@"ipAddress"];
	[encoder encodeObject:desc forKey:@"desc"];
	[encoder encodeBool:self.enabled forKey:@"enabled"];
	[encoder encodeBool:self.useSSL forKey:@"useSSL"];
	[encoder encodeObject:self.uuidString forKey:@"uuidString"];
}


- (void) dealloc
{
	[name release];
	[desc release];
	[ipAddress release];
	[super dealloc];
}

#pragma mark "Refresh (Customer List)"

- (void) refresh
{
	if (self.refreshInProgress) return;
	
	/* Download the Customer List */
	NSString *protocol = self.customer.coreDeployment.useSSL ? @"https" : @"http";
	NSString *port =  self.customer.coreDeployment.useSSL ? @"51143" : @"51180";
	NSURLRequest *theRequest= [NSURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@://%@:%@/console.php", 
																				 protocol, self.ipAddress, port]]
											  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
										  timeoutInterval:60.0];
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		receivedData=[[NSMutableData data] retain];
		self.refreshInProgress = YES;
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}	
}

- (void) forceRefresh
{
    [self refresh];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
    /* Parse XML using TBXML */
    TBXML *xml = [TBXML tbxmlWithXMLData:receivedData];
    LTCustomer *firstCustomer = nil;
    if (xml && xml.rootXMLElement)
    {
        /* Loop through XML nodes */
        TBXMLElement *node = xml.rootXMLElement;
        for (node=node->firstChild; node; node=node->nextSibling)
        {
            NSString *nodeName = [TBXML elementName:node];
            if ([nodeName isEqualToString:@"customer"])
            {
                LTCustomer *curCustomer = [childDict objectForKey:[TBXML textForElementNamed:@"name" parentElement:node]];
                if (!curCustomer)
                {
                    curCustomer = [LTCustomer new];
                    curCustomer.ipAddress = self.ipAddress;
                    curCustomer.coreDeployment = self;
                }
                if (!firstCustomer) firstCustomer = [curCustomer retain];
                
                curCustomer.name = [TBXML textForElementNamed:@"name" parentElement:node];
                curCustomer.desc = curCustomer.name;
                curCustomer.customerName = curCustomer.name;
                curCustomer.url = [TBXML textForElementNamed:@"baseurl" parentElement:node];
                curCustomer.cluster = [TBXML textForElementNamed:@"cluster" parentElement:node];
                curCustomer.node = [TBXML textForElementNamed:@"node" parentElement:node];
                curCustomer.uuidString = [TBXML textForElementNamed:@"uuid" parentElement:node];
                
                if (![children containsObject:curCustomer])
                {
                    [children addObject:curCustomer];
                    [childDict setObject:curCustomer forKey:curCustomer.name];
                    [curCustomer refresh];
                    [curCustomer.incidentList refreshCountOnly];
                    [curCustomer autorelease];	
                    
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"LTCustomerAdded" object:curCustomer];
                }
            }			
        }
    }
	
	/* Clean-up */
    [receivedData release];
	[connection release];
	
	/* Set Core UUID (First customer UUID) */
	if (!self.uuidString && firstCustomer.uuidString)
	{
		AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
		self.uuidString = firstCustomer.uuidString;
		[appDelegate saveCoreDeployments];
		
	}
	[firstCustomer release];
    
    /* Set State */
    self.lastRefresh = [NSDate date];
	self.refreshInProgress = NO;
	
	/* Post Notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"RefreshFinished" object:self];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTCoreDeploymentRefreshFinished" object:self];
}

#pragma mark "Reachability"

- (BOOL) reachable
{
	SCNetworkReachabilityFlags reachabilityFlags;
	BOOL gotFlags = SCNetworkReachabilityGetFlags(reachabilityRef, &reachabilityFlags);
	if (!gotFlags) 
	{
		return NO;	
	}
	
	if (reachabilityFlags & kSCNetworkReachabilityFlagsReachable)
	{
		/* Node is reachable */
		if (reachabilityFlags & kSCNetworkReachabilityFlagsConnectionRequired)
		{
			/* Needs a connection */
			if (reachabilityFlags & kSCNetworkReachabilityFlagsIsWWAN)
			{
				/* Connection is WWAN */
				return YES;
			}
			else
			{
				/* Other connection type needed */
				return NO;
			}
		}
		else
		{ 
			return YES; 
		}
	}
	else
	{
		/* Node is not reachable */
		return NO;
	}	
}


static void reachabilityCallback(SCNetworkReachabilityRef target, SCNetworkReachabilityFlags flags, void *info)
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTCoreDeploymentReachabilityChanged" 
														object:info];
}

- (void) setIpAddress:(NSString *)value
{
	[ipAddress release];
	ipAddress = [value retain];
	
	if (ipAddress && [ipAddress length] > 0)
	{
		struct sockaddr_in address;
		memset((char *) &address, sizeof(struct sockaddr_in), 0);
		address.sin_family = AF_INET;
		address.sin_len = sizeof(struct sockaddr_in);
		
		int conversionResult = inet_aton([ipAddress UTF8String], &address.sin_addr);
		if (conversionResult == 0) 
		{
			/* Not an IP String -- use as hostname */
			reachabilityRef = SCNetworkReachabilityCreateWithName(kCFAllocatorDefault, [self.ipAddress UTF8String]);
		}
		else
		{
			/* IP Address String converted */
			reachabilityRef = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (struct sockaddr *)&address);
		}		
		NSAssert1(reachabilityRef != NULL, @"Failed to create SCNetworkReachabilityRef for host: %@", ipAddress);
		CFRunLoopRef runLoop = [[NSRunLoop currentRunLoop] getCFRunLoop];
		SCNetworkReachabilityContext context = {0, self, NULL, NULL, NULL};
		SCNetworkReachabilitySetCallback(reachabilityRef, reachabilityCallback, &context);
		SCNetworkReachabilityScheduleWithRunLoop(reachabilityRef, runLoop, kCFRunLoopDefaultMode);
	}
}	

- (void) showUnreachableAlert
{
	UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Deployment is Unreachable"
													 message:@"The Lithium Core deployment is currently unreachable, please try again later"
													delegate:nil
										   cancelButtonTitle:@"Dismiss"
										   otherButtonTitles:nil] autorelease];
	[alert show];	
}

#pragma mark "Properties"

@synthesize enabled, useSSL, discovered;
@synthesize refreshFailAlertShown;

@end
