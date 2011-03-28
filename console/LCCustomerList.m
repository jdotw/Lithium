//
//  LCCustomerList.m
//  Lithium Console
//
//  Created by James Wilson on 12/09/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCCustomerList.h"
#import "LCConsoleController.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#import "LCPreferencesController.h"

static LCCustomerList *masterlist = nil;

@implementation LCCustomerList

/* Class Initialisation */

+ (LCCustomerList *) list
{
	return [[[LCCustomerList alloc] init] autorelease];
}

- (LCCustomerList *) init;
{
	[super init];
	
	array = [[NSMutableArray array] retain];
	dict = [[NSMutableDictionary dictionary] retain];
	dynamicDeployments = [[NSMutableArray array] retain];
	dynamicDeploymentDict = [[NSMutableDictionary dictionary] retain];
	staticDeployments = [[NSMutableArray array] retain];
	if ([[NSUserDefaults standardUserDefaults] objectForKey:PREF_CUSTLIST_STATIC_DEPS])
	{ 
		NSData *data = [[NSUserDefaults standardUserDefaults] objectForKey:PREF_CUSTLIST_STATIC_DEPS];
		NSArray *savedDeployments = [NSKeyedUnarchiver unarchiveObjectWithData:data];
		[self willChangeValueForKey:@"staticDeployments"];
		[staticDeployments addObjectsFromArray:savedDeployments];
		[self didChangeValueForKey:@"staticDeployments"];
	}
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:staticDeployments] forKey:PREF_CUSTLIST_STATIC_DEPS];
	[[NSUserDefaults standardUserDefaults] synchronize];
	
	/* Initialise bonjour browser */
	NSNetServiceBrowser *serviceBrowser;
	serviceBrowser = [[NSNetServiceBrowser alloc] init];
	[serviceBrowser setDelegate:self];
	[serviceBrowser searchForServicesOfType:@"_lithium._tcp" inDomain:@""];
	
	/* Loop through static deployments */
	LCCoreDeployment *dep;
	for (dep in staticDeployments)
	{
		[dep refreshDeployment];
	}
	
	return self;
}	

/* Misc class methods */

+ (BOOL) urlconfEnabled 
{ return [[NSUserDefaults standardUserDefaults] boolForKey:PREF_CUSTLIST_URLCONF_BOOL]; }

+ (NSString *) urlconfURL
{ return [[NSUserDefaults standardUserDefaults] stringForKey:PREF_CUSTLIST_URLCONF_URL]; }

+ (BOOL) discoveryEnabled
{ return [[NSUserDefaults standardUserDefaults] boolForKey:PREF_CUSTLIST_DISCOVERY_BOOL]; }

+ (BOOL) alertOnErrorEnabled
{ return [[NSUserDefaults standardUserDefaults] boolForKey:PREF_CUSTLIST_ALERTERROR_BOOL]; }

/* Master list methods */

+ (LCCustomerList *) masterInit 
{
	/* Create and refresh master list */
	if (!masterlist)
	{
		masterlist = [[LCCustomerList alloc] retain];
		[masterlist init];
		[masterlist refresh];
	}
	return masterlist;
}

+ (void) masterRefresh
{ if (masterlist) [masterlist refresh]; }

+ (LCCustomerList *) masterList
{ 
	if (!masterlist)
	{
		[LCCustomerList masterInit];
	}
	return masterlist; 
}

+ (NSMutableArray *) masterArray
{ 
	if (masterlist) return [masterlist array];
	return nil;
}

+ (NSMutableDictionary *) masterDict
{
	if (masterlist) return [masterlist dict];
	return nil;
}

+ (LCCustomer *) masterCustByName:(NSString *)custname
{
	if (masterlist) return [masterlist custByName:custname];
	return nil;
}

/* Customer list instances methods */

- (void) refresh
{
	return;
	
	/* Refresh customer data */
	testMode = NO;

	/* Set Flag */
	[self setRefreshInProgress:YES];

	/* Process static entries */
	for (LCCoreDeployment *deployment in staticDeployments)
	{
		[self refreshFromURL:[deployment url]];
	}

	if ([LCCustomerList discoveryEnabled])
	{
		/* Process dynamic entries */
		for (LCCoreDeployment *deployment in dynamicDeployments)
		{
	 		[self refreshFromURL:[deployment url]];
		}
	}
	
	/* Set Flag */
	[self setRefreshInProgress:NO];
}

- (void) refreshFromURL:(NSString *)urlString
{
	return;
	
	/* Load URL */
	NSError *error = nil;
	NSURLResponse *response = nil;
	if (![urlString hasSuffix:@".xml"] && ![urlString hasSuffix:@".php"])
	{ urlString = [NSString stringWithFormat:@"%@/console.php", urlString]; }
	NSURLRequest *url_req = [NSURLRequest requestWithURL:[NSURL URLWithString:urlString]
											 cachePolicy:NSURLRequestReloadIgnoringCacheData
										 timeoutInterval:5.0];
	NSData *recvd_data = [NSURLConnection sendSynchronousRequest:url_req returningResponse:&response error:&error];
	if (!recvd_data)
	{
		/* Connection failed */
		return;
	}
	
	/* Validate */
	NSString *xmlString = [[[NSString alloc] initWithData:recvd_data encoding:NSUTF8StringEncoding] autorelease];
	if (![xmlString hasPrefix:@"<?xml"])
	{
		return;
	}
	
	/* Parse XML doc */
	curXMLElement = nil;
	element_dict = nil;
	customersFound = 0;
	NSXMLParser *parser = [[NSXMLParser alloc] initWithData:recvd_data];
	[parser setDelegate:self];
	[parser setShouldResolveExternalEntities:YES];
	encounteredError = NO;
	[parser parse];
}

- (BOOL) testUrl:(NSString *)urlString
{
	testMode = YES;
	[self setRefreshInProgress:YES];
	[self refreshFromURL:urlString];
	[self setRefreshInProgress:NO];
	testMode = NO;
	if (customersFound > 0) return YES;
	else return NO;
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for start of a customer */
	if ([element isEqualToString:@"customer"])
	{
		if (element_dict) [element_dict release];
		element_dict = [[NSMutableDictionary dictionary] retain];
	}
	
	/* Check for error */
	if ([element isEqualToString:@"error"])
	{
		/* Encountered Error */
		encounteredError = YES;
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
	/* Add element string to dict */
	if (curXMLElement && curXMLString)
	{ 
		[element_dict setObject:curXMLString forKey:curXMLElement];
	}
	
	/* Check for end of a customer */
	if ([element isEqualToString:@"customer"])
	{
		/* Check we're not in test mode */
		if (!testMode)
		{
			/* Check if customer exists */
			if (![dict objectForKey:[element_dict objectForKey:@"name"]])
			{
				/* Customer does not exist, add it */
				LCCustomer *cust = [LCCustomer customerWithName:[element_dict objectForKey:@"name"]
														cluster:[element_dict objectForKey:@"cluster"]
														   node:[element_dict objectForKey:@"node"]
															url:[element_dict objectForKey:@"baseurl"]];
				if (cust)
				{ 
					[self insertObject:cust inArrayAtIndex:[array count]]; 
					if ([cust name]) [dict setObject:cust forKey:[cust name]];
					[cust setAutoRefresh:YES];
				}
			}
		}
		
		/* Increment found count */
		customersFound++;
		
		/* Release current dictionary */
		[element_dict release];
		element_dict = nil;
	}
	
	/* Check for end of a message (error) */
	
	
	/* Release current element */
	if (curXMLElement)
	{	
		[curXMLElement release];
		curXMLElement = nil;
	}
}

- (void) refreshFinished
{

}

#pragma mark "Bonjour Methods"

- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser
{
}

- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser
{
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
			 didNotSearch:(NSDictionary *)errorDict
{
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		   didFindService:(NSNetService *)netService
			   moreComing:(BOOL)moreComing
{
	[netService retain];
	[netService setDelegate:self];
	[netService resolveWithTimeout:10.0];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		 didRemoveService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing
{
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
}

- (void)netServiceDidResolveAddress:(NSNetService *)sender
{
    NSData             *address = nil;
    struct sockaddr_in *socketAddress = nil;
    NSString           *ipString = nil;
    int                port;
	int i;
	
    for (i = 0; i < [[sender addresses] count]; i++)
    {
        address = [[sender addresses] objectAtIndex:i];
        socketAddress = (struct sockaddr_in *)
			[address bytes];
        ipString = [NSString stringWithFormat: @"%s", inet_ntoa (socketAddress->sin_addr)];
		port = socketAddress->sin_port;
		if (![ipString isEqualToString:@"0.0.0.0"])
		{
			LCCoreDeployment *deployment = [[[LCCoreDeployment alloc] init] autorelease];
			deployment.address = ipString;
			deployment.port = ntohs(port);
			if (deployment.port == 51143 || deployment.port == 443) deployment.useSSL = YES;
			deployment.isDynamic = YES;
			if ([dynamicDeploymentDict objectForKey:[deployment url]])
			{
				/* Refresh existing */
				if ([LCCustomerList discoveryEnabled])
				{ [(LCCoreDeployment *)[dynamicDeploymentDict objectForKey:[deployment url]] refreshDeployment]; }
			}
			else
			{ [self insertObject:deployment inDynamicDeploymentsAtIndex:[dynamicDeployments count]]; }
		}
	}
}

/* Variable retrieval methods */

- (NSMutableArray *) array
{ return array; }
- (void) setArray:(NSArray *)newArray
{
	if (array) [array release];
	array = [newArray retain];
}

- (void) insertObject:(LCCustomer *)cust inArrayAtIndex:(unsigned int)index
{
	[array insertObject:cust atIndex:index];
}

- (void) removeObjectFromArrayAtIndex:(unsigned int)index
{
	[array removeObjectAtIndex:index];
}

- (NSMutableDictionary *) dict
{ return dict; }
- (void) setDict:(NSDictionary *)newDict
{ 
	if (dict) [dict release];
	dict = [newDict retain];
}

- (LCCustomer *) custByName:(NSString *)custname
{
	if (!dict) return nil;
	return [dict objectForKey:custname];
}

- (NSMutableArray *) staticDeployments
{ return staticDeployments; }

- (void) insertObject:(LCCoreDeployment *)deployment inStaticDeploymentsAtIndex:(unsigned int)index
{
	[staticDeployments insertObject:deployment atIndex:index];
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:staticDeployments] forKey:PREF_CUSTLIST_STATIC_DEPS];
	[[NSUserDefaults standardUserDefaults] synchronize];
	[deployment refreshDeployment];
}

- (void) removeObjectFromStaticDeploymentsAtIndex:(unsigned int)index
{
	[staticDeployments removeObjectAtIndex:index];
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:staticDeployments] forKey:PREF_CUSTLIST_STATIC_DEPS];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (NSMutableArray *) dynamicDeployments
{ return dynamicDeployments; }

- (void) insertObject:(LCCoreDeployment *)deployment inDynamicDeploymentsAtIndex:(unsigned int)index
{
	[dynamicDeploymentDict setObject:deployment forKey:[deployment url]];
	[dynamicDeployments insertObject:deployment atIndex:index];
	if ([LCCustomerList discoveryEnabled]) [deployment refreshDeployment];
}

- (void) removeObjectFromDynamicDeploymentsAtIndex:(unsigned int)index
{
	[dynamicDeploymentDict removeObjectForKey:[[dynamicDeployments objectAtIndex:index] url]];
	[dynamicDeployments removeObjectAtIndex:index];
}

- (BOOL) refreshInProgress
{ return refreshInProgress; }

- (void) setRefreshInProgress:(BOOL)value
{ refreshInProgress = value; }

#pragma mark Browser Outlineview Compatibility

- (NSArray *) children
{ return array; }

- (NSString *) name
{ return @"Customers"; }
- (NSString *) desc
{ return @"Customers"; }

- (BOOL) childrenPopulated
{ return childrenPopulated; }

- (void) setChildrenPopulated:(BOOL)flag
{ childrenPopulated = flag; }

@synthesize staticDeployments;
@synthesize dynamicDeployments;
@synthesize dynamicDeploymentDict;
@synthesize element_dict;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize testMode;
@synthesize customersFound;
@synthesize encounteredError;
@synthesize msession;
@synthesize window;
@synthesize pbar;
@end
