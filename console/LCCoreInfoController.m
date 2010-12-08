//
//  LCCoreInfoController.m
//  Lithium Console
//
//  Created by James Wilson on 25/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#include <sys/time.h>

#import "LCCoreInfoController.h"


@implementation LCCoreInfoController

#pragma mark "Constructors"

- (void) awakeFromNib
{
	[self setProperties:[NSMutableDictionary dictionary]];
	[self setResources:[NSMutableArray array]];
	[self setResourceDictionary:[NSMutableDictionary dictionary]];
	[self highPriorityRefresh];
	[self checkCurrentVersion];
	
	uptimeUpdateTimer = [[NSTimer scheduledTimerWithTimeInterval:1.0
														  target:self
														selector:@selector(updateUptime)
														userInfo:nil
														 repeats:YES] retain];
}

- (void) dealloc
{
	[uptimeUpdateTimer invalidate];
	[uptimeUpdateTimer release];
	if (refreshXMLRequest)
	{ [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	[self setResources:nil];
	[self setResourceDictionary:nil];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	NSString *resAddress = [NSString stringWithFormat:@"%@:%@:0:0:", 
		[[[setupController customer] resourceAddress] cluster], [[[setupController customer] resourceAddress] node]];
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:[setupController customer]
												  resource:[LCResourceAddress addressWithString:resAddress] 
													entity:nil
												   xmlname:@"resource_list"
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setXMLDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	[self setRefreshInProgress:YES];
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) XMLRequestPreParse:(id)sender
{
	/* Set current element to nil */
	curXMLElement = nil;
	
	/* Create XML actions dictionary */
	xmlResDictionary = [[NSMutableDictionary dictionary] retain];
}

- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict 
{
	curXMLElement = [element retain];
	
	/* Check for start of a case */
	if ([element isEqualToString:@"resource"])
	{
		if (curXMLDictionary) [curXMLDictionary release];
		curXMLDictionary = [[NSMutableDictionary dictionary] retain];
	}
	
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
	/* Add element string to dict */
	if (curXMLElement && curXMLString)
	{ 
		if (curXMLDictionary) [curXMLDictionary setObject:curXMLString forKey:curXMLElement];
		else [properties setObject:curXMLString forKey:curXMLElement];
	}

	/* Check for start time */
	if ([element isEqualToString:@"starttime"])
	{
		[self setStartTime:curXMLString]; 
	}
	
	/* Release current element */
	if (curXMLElement)
	{
		[curXMLElement release];
		curXMLElement = nil;
	}
	
	/* Check for end of a resource */
	if ([element isEqualToString:@"resource"])
	{
		/* Create resource */
		LCCoreInfoResource *res = [LCCoreInfoResource resourceWithProperties:curXMLDictionary];
		[xmlResDictionary setObject:res forKey:[res id]];

		/* Check for existing res */
		if (![resourceDictionary objectForKey:[res id]])
		{
			/* Insert New Resource */
			[self insertObject:res inResourcesAtIndex:[resources count]]; 
		}
		else
		{
			/* Update existing */
			LCCoreInfoResource *existingRes = [resourceDictionary objectForKey:[res id]];
			NSEnumerator *propertiesEnum = [curXMLDictionary keyEnumerator];
			NSString *propertyKey;
			while (propertyKey=[propertiesEnum nextObject])
			{ [[existingRes properties] setObject:[curXMLDictionary objectForKey:propertyKey] forKey:propertyKey]; }
		}
		
		/* Release current dictionary */
		[curXMLDictionary release];
		curXMLDictionary = nil;
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	unsigned int i;
	
	/* Check success */
	if ([sender success])
	{
		/* Remove extinct resources */
		for (i=0; i < [resources count]; i++)
		{
			LCCoreInfoResource *res = [resources objectAtIndex:i];
			if (![xmlResDictionary objectForKey:[res id]])
			{
				/* Res is extinct, remove it */
				[self removeObjectFromResourcesAtIndex:i];
				i--;
			}
		}
	}
	
	/* Free xml case dictionary */
	if (xmlResDictionary) [xmlResDictionary release];
	xmlResDictionary = nil;
	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set Flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "Current Version Check"

- (void) checkCurrentVersion
{
	NSString *urlString = [NSString stringWithFormat:@"http://secure.lithiumcorp.com.au/appcast/core_osx_curver.xml"];
	NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															  cachePolicy:NSURLRequestReloadIgnoringCacheData
														  timeoutInterval:[[NSUserDefaults standardUserDefaults] floatForKey:@"xmlHTTPTimeoutSec"]];
	
	urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
	if (urlConn)
	{
		/* Connection proceeding */
		receivedData = [[NSMutableData data] retain];
	}
	else
	{ 
		/* Connection failed */
	}	
	
	return;
}

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
}		

-(void) connectionDidFinishLoading:(NSURLConnection *) connection
{
	/* Download finished, parse the XML */
	NSXMLParser *parser;
	
	/* Validate */
	NSString *xmlString = [[[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding] autorelease];
	if ([xmlString hasPrefix:@"<?xml"])
	{
		/* Parse XML doc */
		parser = [[[NSXMLParser alloc] initWithData:receivedData] autorelease];
		[parser setDelegate:self];
		[parser setShouldResolveExternalEntities:YES];
		refreshLocal = NO;
		[parser parse];
	}
	
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
} 

#pragma mark "UI Actions"

- (IBAction) downloadUpdateClicked:(id)sender
{
	
}

- (IBAction) restartProcessesClicked:(id)sender
{
	[NSApp beginSheet:restartSheet
	   modalForWindow:[setupController window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];		
}

- (IBAction) restartSheetRestartClicked:(id)sender
{
	[[setupController customer] restartMonitoringProcess];	
	[NSApp endSheet:restartSheet];
	[restartSheet close];
}

- (IBAction) restartSheetCancelClicked:(id)sender
{
	[NSApp endSheet:restartSheet];
	[restartSheet close];
}

#pragma mark "Uptime"

- (void) updateUptime
{
	struct timeval now;
	gettimeofday (&now, NULL);
	if ([[self startTime] floatValue] > (float) now.tv_sec)
	{ [self setUptime:@"0"]; }
	float interval = (float) now.tv_sec - [[self startTime] floatValue];
	[self setUptime:[NSString stringWithFormat:@"%.0f", interval]];
}	

#pragma mark "Accessors"

- (NSMutableDictionary *) properties
{ return properties; }

- (void) setProperties:(NSMutableDictionary *)dict
{
	[properties release];
	properties = [dict retain];
}

- (NSMutableArray *) resources
{ return resources; }

- (void) setResources:(NSMutableArray *)array
{
	[resources release];
	resources = [array retain];
}

- (void) insertObject:(LCCoreInfoResource *)resource inResourcesAtIndex:(unsigned int)index
{
	[resources insertObject:resource atIndex:index];
	[resourceDictionary setObject:resource forKey:[resource id]];
}

- (void) removeObjectFromResourcesAtIndex:(unsigned int)index
{
	[resourceDictionary removeObjectForKey:[[resources objectAtIndex:index] id]];
	[resources removeObjectAtIndex:index];
}

- (NSMutableDictionary *) resourceDictionary
{ return resourceDictionary; }

- (void) setResourceDictionary:(NSMutableDictionary *)dict
{
	[resourceDictionary release];
	resourceDictionary = [dict retain];
}

- (BOOL) refreshInProgress
{ return refreshInProgress; }

- (void) setRefreshInProgress:(BOOL)flag
{ refreshInProgress = flag; }

- (NSString *) startTime
{ return startTime; }

- (void) setStartTime:(NSString *)string
{
	[startTime release];
	startTime = [string retain];
}

- (NSString *) uptime
{ return uptime; }

- (void) setUptime:(NSString *)string
{
	[uptime release];
	uptime = [string retain];
}

@synthesize setupController;
@synthesize restartSheet;
@synthesize refreshTimer;
@synthesize refreshXMLRequest;
@synthesize curXMLDictionary;
@synthesize curXMLElement;
@synthesize curXMLString;
@synthesize xmlResDictionary;
@synthesize urlConn;
@synthesize receivedData;
@synthesize refreshLocal;
@synthesize uptimeUpdateTimer;
@end
