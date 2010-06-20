//
//  LCCoreDeployment.m
//  Lithium Console
//
//  Created by James Wilson on 23/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <sys/socket.h>
#import <netinet/in.h>
#import <netinet6/in6.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#include <netdb.h>

#import "LCCoreDeployment.h"
#import "LCCustomerList.h"
#import "LCPreferencesController.h"
#import "LCConsoleController.h"
#import "LCError.h"
#import "NSURLRequest-SSLHack.h"
#import <SystemConfiguration/SystemConfiguration.h>
#import <SystemConfiguration/SCNetworkReachability.h>

@implementation LCCoreDeployment

#pragma mark "Constructors"

- (LCCoreDeployment *) init
{
	[super init];

	/* Create variables */
	properties = [[NSMutableDictionary dictionary] retain];
	customers = [[NSMutableArray array] retain];
	
	/* Set Defaults */
	self.statusInteger = -1;
	self.port = 51180;
	
	return self;
}

- (LCCoreDeployment *) initWithCoder:(NSCoder *)decoder
{
	[self init];

	/* Get Properties */
	self.properties = [decoder decodeObjectForKey:@"properties"];
	
	/* Set properties manually to ensure accessor actions are performed */
	for (NSString *key in [self.properties allKeys])
	{
		[self setValue:[self.properties objectForKey:key] forKey:key];
	}
	
	/* Legacy -- Check for URL */
	if ([properties objectForKey:@"url"])
	{
		NSURL *url = [NSURL URLWithString:[properties objectForKey:@"url"]];
		self.address = [url host];
		if ([url port]) self.port = [[url port] intValue];
		else self.port = 51180;
		[properties removeObjectForKey:@"url"];
	}
	
    return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

- (void) dealloc
{
	if (reachabilityRef)
	{
		CFRunLoopRef runLoop = [[NSRunLoop currentRunLoop] getCFRunLoop];
		SCNetworkReachabilityUnscheduleFromRunLoop(reachabilityRef, runLoop, kCFRunLoopDefaultMode);
		CFRelease(reachabilityRef);
	}
	[properties release];
	[super dealloc];
}

#pragma mark "Test Deployment"

- (void) refreshDeployment
{
	if (urlConn) return;

	if (testMode)
	{ [self setStatus:@"Testing..."]; }
	else
	{ [self setStatus:@"Downloading Customer List..."]; }
	[self setStatusInteger:1];
	[self setRefreshInProgress:YES];

	/* Check suffix */
	NSString *urlString;
	if (![[self url] hasSuffix:@".xml"] && ![[self url] hasSuffix:@".php"])
	{ urlString = [NSString stringWithFormat:@"%@/console.php", [self url]]; }
	else
	{ urlString = [NSString stringWithString:[self url]]; }
		
	/* Use NSURLConnection to download the xml */
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:5.0];
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Set up data */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{
		/* Error occurred */
		[self setRefreshInProgress:NO];
		[self setStatus:@"Failed to retrieve deployment status"];
		[self setStatusInteger:3];
		[self setStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		[self setErrorEncountered:YES]; 
		[LCError logError:@"Failed to retrieve deployment status"
			   forCustomer:nil
				  fullText:[NSString stringWithFormat:@"URL Queried: %@", urlString]];
		if (!testMode) [self displayAlert];
	}
}

- (void) testDeployment
{
	testMode = YES;
	[self refreshDeployment];
}

#pragma mark "NSURLConnection Delegates (Image Download)"

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
	/* String */
	NSString *dataString = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	
	if ([dataString hasPrefix:@"<?xml"])
	{
		/* Parse */
		NSXMLParser *parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:NO];
		[self setErrorEncountered:NO];
		[parser parse];

		/* Check result */
		if (![self errorEncountered])
		{ 
			[self setStatus:@"OK"]; 
			[self setStatusInteger:0];
			[self setStatusIcon:[NSImage imageNamed:@"ok_48.tif"]];
		}
	}
	else
	{
		/* Server did not return XML */
		[self setStatusInteger:3];
		[self setStatus:@"XML not found in server response"];
		[self setStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		[self setErrorEncountered:YES]; 
		[LCError logError:[NSString stringWithFormat:@"Error in XML Customer List from %@", [self url]]
			  forCustomer:nil
				 fullText:[NSString stringWithFormat:@"Server Returned: ", dataString]];
		if (!testMode) [self displayAlert];
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];

	/* Set refrsh flag */
	[self setRefreshInProgress:NO];
	testMode = NO;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	if ([[error domain] isEqualToString:NSURLErrorDomain] && [error 
		code] <= NSURLErrorServerCertificateHasBadDate && [error code] >= 
		NSURLErrorServerCertificateNotYetValid)    // handle certificate failures
	{
//		/* A certificate error occurred... prompt the 
//		 * user to see if they would like to attempt to 
//		 * add it as a trusted cert 
//		 */
//		NSString *message = [NSString stringWithFormat:@"Server certificate error at %@", [self url]];
//		NSAlert *alert = [NSAlert alertWithMessageText:message
//										 defaultButton:@"Trust Certificate"
//									   alternateButton:@"Open Preferences"
//										   otherButton:@"Do Not Trust"
//							 informativeTextWithFormat:@"The Lithium Core deployment at %@ is using an SSL Certificate that has been identified as being potentially untrustworthy.\n\nThis is normal for Lithium Core deployments using the default self-signed SSL certificate.\n\nIf you are confident that the Lithium Core deployment at %@ can be trusted, click 'Trust Certificate' below to add the server's SSL certificate to the list of trusted certificates.", [self url], [self url]];
//		[alert setAlertStyle:NSCriticalAlertStyle];
//		int button = [alert runModal];
//		if (button == NSAlertDefaultReturn)
//		{			
			NSURL *failingURL = [[error userInfo] objectForKey:@"NSErrorFailingURLKey"];
//			NSArray *badCerts = [[error userInfo] objectForKey:@"NSErrorPeerCertificateChainKey"];
//			SecPolicySearchRef policySearch;
//			if (SecPolicySearchCreate(CSSM_CERT_X_509v3, &CSSMOID_APPLE_TP_SSL, 
//									  NULL, &policySearch) == noErr)
//			{
//				SecPolicyRef policy;
//				while (SecPolicySearchCopyNext(policySearch, &policy) == noErr)    // this should only go through once
//				{
//					SecTrustRef trust;
//					if (SecTrustCreateWithCertificates((CFArrayRef)badCerts, policy, &trust) == noErr)
//					{
//						CFRelease(trust);
//						CFRelease(policy);
//						CFRelease(policySearch);
						[NSURLRequest setAllowsAnyHTTPSCertificate:YES forHost:[failingURL host]];
						[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(refreshDeployment) userInfo:nil repeats:NO];
//						break;
//					}				
//				}
//			}
//		}
//		else if (button == NSAlertAlternateReturn)
//		{ [[LCPreferencesController alloc] init]; }
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];

	/* Set Status */
	[self setStatusInteger:3];
	[self setStatus:[[error localizedDescription] capitalizedString]];
	[self setStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
	[self setErrorEncountered:YES]; 
	if (!testMode) [self displayAlert];
	[LCError logError:[NSString stringWithFormat:@"Failed to retrieve XML Customer List from %@", [self url]]
		  forCustomer:nil
			 fullText:[NSString stringWithFormat:@"Error: %@", [[error localizedDescription] capitalizedString]]];
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
	testMode = NO;
}

#pragma mark "XML Parsing"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	/* Free previous curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
	
	/* Check for start of customer */
	if ([element isEqualToString:@"customer"])
	{
		curXMLDictionary = [[NSMutableDictionary dictionary] retain];
	}
	
	/* Check for start of error */
	if ([element isEqualToString:@"error"])
	{
		[self setStatus:@"Unknown Error"];
		[self setErrorEncountered:YES]; 
		[self setStatusInteger:3];
		[self setStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		if (!testMode) [self displayAlert];
	}
}

- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	/* Append characters to curXMLString */
	if (!curXMLString)
	{ curXMLString = [[NSMutableString stringWithString:string] retain]; }
	else
	{ [curXMLString appendString:string]; }
}

- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname
{
	/* Default : Add element string to dict */
	if (curXMLString)
	{ 
		[curXMLDictionary setObject:curXMLString forKey:element];
	}	
	
	/* Check for message (error) */
	if ([element isEqualToString:@"message"])
	{
		/* Error message encountered */
		[self setStatus:curXMLString];
	}

	/* Check for status (error) */
	else if ([element isEqualToString:@"status"])
	{
		/* Error message encountered */
		[self setStatusInteger:[curXMLString intValue]];
	}

	/* Check for status (error) */
	else if ([element isEqualToString:@"version"])
	{
		/* Error message encountered */
		[self setVersion:curXMLString];
	}	
	
	/* Check for end of a customer */
	else if ([element isEqualToString:@"customer"])
	{
		/* Check if customer exists */
		if (![[LCCustomerList masterDict] objectForKey:[curXMLDictionary objectForKey:@"name"]] && !testMode)
		{
			/* Customer does not exist, add it */
			LCCustomer *cust = [LCCustomer customerWithName:[curXMLDictionary objectForKey:@"name"]
													cluster:[curXMLDictionary objectForKey:@"cluster"]
													   node:[curXMLDictionary objectForKey:@"node"]
														url:[curXMLDictionary objectForKey:@"baseurl"]];
			if (cust)
			{
				[[LCCustomerList masterList] insertObject:cust inArrayAtIndex:[[LCCustomerList masterArray] count]]; 
				if ([cust name]) [[LCCustomerList masterDict] setObject:cust forKey:[cust name]];
				[cust setAutoRefresh:YES];
				if (![customers containsObject:cust])
				{ [self insertObject:cust inCustomersAtIndex:[customers count]]; }
			}
		}
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
	
	/* Free current curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

#pragma mark "Alert"

- (void) displayAlert
{
	if ([LCCustomerList alertOnErrorEnabled])
	{
		NSAlert *alert = [NSAlert alertWithMessageText:@"Console has detected a problem with a Lithium Core deployment."
						defaultButton:@"Open Preferences"
					  alternateButton:@"Disable Alerts"
						  otherButton:@"Ignore"
			 informativeTextWithFormat:@"Console has detected a problem with the Lithium Core deployment at %@. To view more information about the problem, click 'Open Preferences'", [self url]];
		[alert setIcon:[NSImage imageNamed:@"stop_48.tif"]];
		int button = [alert runModal];
		if (button == NSAlertDefaultReturn)
		{
			/* Open Preferences */
			[LCPreferencesController preferencesController];
		}
		else if (button == NSAlertAlternateReturn)
		{
			/* Always ignore */
			[[NSUserDefaults standardUserDefaults] setBool:NO forKey:PREF_CUSTLIST_ALERTERROR_BOOL]; 
			[[NSUserDefaults standardUserDefaults] synchronize];
		}
	}
}
#pragma mark "Reachability Methods"

static void reachabilityCallback(SCNetworkReachabilityRef target, SCNetworkConnectionFlags flags, void *info)
{
	LCCoreDeployment *core = (LCCoreDeployment *)info;
	SCNetworkConnectionFlags reachabilityFlags;
	BOOL gotFlags = SCNetworkReachabilityGetFlags(target, &reachabilityFlags);
	if (!gotFlags) 
	{
		core.reachable = NO;
	}
	
	if (reachabilityFlags & kSCNetworkFlagsReachable)
	{
		/* Node is reachable */
		if (reachabilityFlags & kSCNetworkFlagsConnectionRequired)
		{
			/* Needs a connection */
			if (reachabilityFlags & kSCNetworkFlagsConnectionAutomatic)
			{
				/* Connection is automatic */
				core.reachable = YES;
			}
			else
			{
				/* Other connection type needed */
				core.reachable = NO;
			}
		}
		else
		{ 
			core.reachable = YES; 
		}
	}
	else
	{
		/* Node is not reachable */
		core.reachable = NO;
	}	
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LCCoreDeploymentReachabilityChanged" 
														object:nil];
	
	if (core.reachable)
	{ [[LCCustomerList masterList] refreshFromURL:core.url]; }
}

@synthesize reachable;

#pragma mark "Accessors"

@synthesize properties;
- (void) setProperties:(NSMutableDictionary *)dictionary
{
	[properties release];
	properties = [dictionary mutableCopy];
}

- (NSString *) url
{ 
	if (self.useSSL) return [NSString stringWithFormat:@"https://%@:%i/", self.address, self.port];
	else return [NSString stringWithFormat:@"http://%@:%i/", self.address, self.port];
}

- (NSString *) address
{
	return [properties objectForKey:@"address"];
}

- (void) setAddress:(NSString *)value
{
	if (![value isEqualToString:self.address])
	{
		[properties setObject:value forKey:@"address"];
		[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:[[LCCustomerList masterList] staticDeployments]] forKey:PREF_CUSTLIST_STATIC_DEPS];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
	
	if (self.address && [self.address length] > 0)
	{
		struct sockaddr_in address;
		memset((char *) &address, sizeof(struct sockaddr_in), 0);
		address.sin_family = AF_INET;
		address.sin_len = sizeof(struct sockaddr_in);
		
		int conversionResult = inet_aton([self.address UTF8String], &address.sin_addr);
		if (conversionResult == 0) 
		{
			/* Not an IP String -- use as hostname */
			reachabilityRef = SCNetworkReachabilityCreateWithName(kCFAllocatorDefault, [self.address UTF8String]);
		}
		else
		{
			/* IP Address String converted */
			reachabilityRef = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (struct sockaddr *)&address);
		}
		if (reachabilityRef)
		{
			CFRunLoopRef runLoop = [[NSRunLoop currentRunLoop] getCFRunLoop];
			SCNetworkReachabilityContext context = {0, self, NULL, NULL, NULL};
			SCNetworkReachabilitySetCallback(reachabilityRef, reachabilityCallback, &context);
			SCNetworkReachabilityScheduleWithRunLoop(reachabilityRef, runLoop, kCFRunLoopDefaultMode);
		}
		else
		{
			NSLog (@"ERROR: Failed to create reachabilityRef for Core %@", self);
			self.reachable = YES;
		}
		
		reachabilityCallback (reachabilityRef, 0, self);
	}		
}

- (int) port
{ 
	return [[properties objectForKey:@"port"] intValue];
}

- (void) setPort:(int)value
{
	if (value != self.port)
	{
		[properties setObject:[NSNumber numberWithInt:value] forKey:@"port"];
		[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:[[LCCustomerList masterList] staticDeployments]] forKey:PREF_CUSTLIST_STATIC_DEPS];
		[[NSUserDefaults standardUserDefaults] synchronize];
		[[LCCustomerList masterList] refreshFromURL:[self url]];
	}
}

- (BOOL) useSSL
{ 
	return [[properties objectForKey:@"useSSL"] boolValue];
}

- (void) setUseSSL:(BOOL)value
{
	if (value != self.useSSL)
	{
		[properties setObject:[NSNumber numberWithBool:value] forKey:@"useSSL"];
		[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:[[LCCustomerList masterList] staticDeployments]] forKey:PREF_CUSTLIST_STATIC_DEPS];
		[[NSUserDefaults standardUserDefaults] synchronize];
		[[LCCustomerList masterList] refreshFromURL:[self url]];
	}
}

@synthesize version;
@synthesize status;
@synthesize isDynamic;
@synthesize refreshInProgress;
@synthesize statusInteger;
@synthesize statusIcon;
@synthesize errorEncountered;
- (BOOL) isCustomer
{ return NO; }
@synthesize disabled;
- (NSImage *) discoveredIcon
{
	if ([self isDynamic]) { return [NSImage imageNamed:@"network_16.tif"]; }
	else return nil;
}
- (BOOL) isDeployment
{ return YES; }

@synthesize customers;
- (void) insertObject:(LCCustomer *)customer inCustomersAtIndex:(unsigned int)index
{
	[customers insertObject:customer atIndex:index];
}
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index
{
	[customers removeObjectAtIndex:index];
}

- (NSString *) displayString
{
	return [NSString stringWithFormat:@"%@:%i", self.address, self.port];
}

- (NSString *) coreVersion
{ 
	return nil;
}

@end
