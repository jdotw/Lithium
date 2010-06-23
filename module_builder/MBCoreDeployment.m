//
//  MBCoreDeployment.m
//  ModuleBuilder
//
//  Created by James Wilson on 22/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBCoreDeployment.h"
#import "NSURLRequest-SSLHack.h"

@implementation MBCoreDeployment

#pragma mark "Constructors"

- (MBCoreDeployment *) init
{
	[super init];
	
	self.customers = [[NSMutableArray array] retain];
	self.customerDict = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (MBCoreDeployment *) initWithCoder:(NSCoder *)decoder
{
	[self init];
	
	self.ipAddress = [decoder decodeObjectForKey:@"ipAddress"];
	if ([decoder decodeObjectForKey:@"customers"])
	{ self.customers = [decoder decodeObjectForKey:@"customers"]; }
	self.username = [decoder decodeObjectForKey:@"username"];
	self.password = [decoder decodeObjectForKey:@"password"];
	self.selected = [decoder decodeBoolForKey:@"selected"];
	
	NSString *selectedCustomerName = [decoder decodeObjectForKey:@"selectedCustomerName"];

	for (MBCoreCustomer *customer in customers)
	{
		[customerDict setObject:customer forKey:customer.name];
		if ([customer.name isEqualToString:selectedCustomerName])
		{ 
			self.selectedCustomer = customer;
			break;
		}
	}
	
	return self;
}

- (void) encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:ipAddress forKey:@"ipAddress"];
	[encoder encodeObject:customers forKey:@"customers"];
	[encoder encodeObject:username forKey:@"username"];
	[encoder encodeObject:password forKey:@"password"];
	if (selectedCustomer)
	{ [encoder encodeObject:selectedCustomer.name forKey:@"selectedCustomerName"]; }
	[encoder encodeBool:selected forKey:@"selected"];
}

#pragma mark "Refresh Deployment"

- (void) refreshDeployment
{
	if (urlConn) return;
	
	[self setStatus:@"Downloading Customer List..."];
	[self setStatusInteger:1];
	[self setRefreshInProgress:YES];
	
	/* Check suffix */
	NSString *urlString = [NSString stringWithFormat:@"http://%@:51180/console.php", [self ipAddress]];
	
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
	}
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
			if (newlyAdded) [self setSelected:YES];
		}
	}
	else
	{
		/* Server did not return XML */
		[self setStatusInteger:3];
		[self setStatus:@"XML not found in server response"];
		[self setStatusIcon:[NSImage imageNamed:@"stop_48.tif"]];
		[self setErrorEncountered:YES]; 
	}
	
	/* Release data */
	[receivedData release];
	receivedData = nil;
	
	/* Release connection */
	urlConn = nil;
	[connection release];
	
	/* Set refrsh flag */
	[self setRefreshInProgress:NO];
	[self setNewlyAdded:NO];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	if ([[error domain] isEqualToString:NSURLErrorDomain] && [error code] <= NSURLErrorServerCertificateHasBadDate && [error code] >= NSURLErrorServerCertificateNotYetValid)    // handle certificate failures
	{
		/* A certificate error occurred... prompt the 
		 * user to see if they would like to attempt to 
		 * add it as a trusted cert 
		 */
		NSString *message = [NSString stringWithFormat:@"Server certificate error at %@", [self ipAddress]];
		NSAlert *alert = [NSAlert alertWithMessageText:message
										 defaultButton:@"Trust Certificate"
									   alternateButton:nil
										   otherButton:@"Do Not Trust"
							 informativeTextWithFormat:@"The Lithium Core deployment at %@ is using HTTPS with an SSL Certificate that has been identified as being potentially untrustworthy.\n\nThis is usually caused by using self-signed certificates on the server.\n\nIf you are confident that the Lithium Core deployment at %@ can be trusted, click 'Trust Certificate' below to add the server's SSL certificate to the list of trusted certificates.", [self ipAddress], [self ipAddress]];
		[alert setIcon:[NSImage imageNamed:@"stop_48.tif"]];
		int button = [alert runModal];
		if (button == NSAlertDefaultReturn)
		{			
			NSURL *failingURL = [[error userInfo] objectForKey:@"NSErrorFailingURLKey"];
			NSArray *badCerts = [[error userInfo] objectForKey:@"NSErrorPeerCertificateChainKey"];
			SecPolicySearchRef policySearch;
			if (SecPolicySearchCreate(CSSM_CERT_X_509v3, &CSSMOID_APPLE_TP_SSL, 
									  NULL, &policySearch) == noErr)
			{
				SecPolicyRef policy;
				while (SecPolicySearchCopyNext(policySearch, &policy) == noErr)    // this should only go through once
				{
					SecTrustRef trust;
					if (SecTrustCreateWithCertificates((CFArrayRef)badCerts, policy, &trust) == noErr)
					{
						CFRelease(trust);
						CFRelease(policy);
						CFRelease(policySearch);
						[NSURLRequest setAllowsAnyHTTPSCertificate:YES forHost:[failingURL host]];
						[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(refreshDeployment) userInfo:nil repeats:NO];
						break;
					}				
				}
			}
		}
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
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "XML Parsing"

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
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
	/* Default : Add element string to dict */
	if (curXMLElement && curXMLString)
	{ 
		[curXMLDictionary setObject:curXMLString forKey:curXMLElement];
	}	
	
	/* Check for end of a customer */
	else if ([element isEqualToString:@"customer"])
	{
		/* Check if customer exists */
		if ([customerDict objectForKey:[curXMLDictionary objectForKey:@"name"]])
		{
			/* Customer exists, update it */
			MBCoreCustomer *cust = [customerDict objectForKey:[curXMLDictionary objectForKey:@"name"]];
			cust.cluster = [curXMLDictionary objectForKey:@"cluster"];
			cust.node = [curXMLDictionary objectForKey:@"node"];
			cust.url = [curXMLDictionary objectForKey:@"baseurl"];
		}
		else
		{
			/* Customer does not exist, add it */
			MBCoreCustomer *cust = [MBCoreCustomer customerWithName:[curXMLDictionary objectForKey:@"name"]
													cluster:[curXMLDictionary objectForKey:@"cluster"]
													   node:[curXMLDictionary objectForKey:@"node"]
														url:[curXMLDictionary objectForKey:@"baseurl"]];
			[self insertObject:cust inCustomersAtIndex:[customers count]];
			if ([cust name]) [customerDict setObject:cust forKey:[cust name]];				
			
			if (!self.selectedCustomer)
			{ self.selectedCustomer = cust; }
		}
		
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
	
	/* Free current curXMLString */
	if (curXMLString)
	{
		[curXMLString release];
		curXMLString = nil;
	}
}

#pragma mark "Properties"
@synthesize ipAddress;
- (void) setIpAddress:(NSString *)value
{
	[ipAddress release];
	ipAddress = [value retain];
	if (self.ipAddress) [self refreshDeployment];
}
@synthesize customers;
- (void) setCustomers:(NSMutableArray *)value
{
	customers = [value mutableCopy];
}
@synthesize customerDict;
- (void) setCustomerDict:(NSMutableDictionary *)value
{
	customerDict = [value mutableCopy];
}
- (void) insertObject:(MBCoreCustomer *)obj inCustomersAtIndex:(unsigned int)index
{
	[customers insertObject:obj atIndex:index];
}
- (void) removeObjectFromCustomersAtIndex:(unsigned int)index
{
	[customers removeObjectAtIndex:index];
}
@synthesize selectedCustomer;
- (void) setSelectedCustomer:(MBCoreCustomer *)value
{
	[selectedCustomer release];
	selectedCustomer = [value retain];
}
@synthesize username;
@synthesize password;
@synthesize status;
@synthesize statusInteger;
@synthesize statusIcon;
@synthesize refreshInProgress;
@synthesize errorEncountered;
@synthesize selected;
@synthesize newlyAdded;

@end
