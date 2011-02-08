//
//  LTIncidentList.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTIncidentList.h"

#import "LTCustomer.h"
#import "LTEntityDescriptor.h"
#import "LTAuthenticationTableViewController.h"
#import "AppDelegate.h"
#import "LTAction.h"
#import "LCXMLParseOperation.h"
#import "LCXMLNode.h"

#define REQ_COUNTANDVERSION 1
#define REQ_LIST 2

@implementation LTIncidentList

#pragma mark "Constructors"

- (LTIncidentList *) init
{
	[super init];
	incidents = [[NSMutableArray array] retain];
	incidentDict = [[NSMutableDictionary dictionary] retain];
	historicList = NO;
	maxResultsCount = 50;
	return self;
}

- (void) dealloc
{
	[incidents release];
	[incidentDict release];
	[super dealloc];
}

#pragma mark -
#pragma mark Refresh

#pragma mark Private API Methods

- (void) _requestCountAndVersion
{
	/* Refresh the incident list */
	LTCustomer *cust = self.customer;
	NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[cust urlForXml:@"incident_list_version" timestamp:0]
															  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														  timeoutInterval:60.0];	
	
	/* Establish Connection */
	NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		refreshInProgress = YES;
		currentRequest = REQ_COUNTANDVERSION;
		receivedData=[[NSMutableData data] retain];
		
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}	
}

- (void) _requestIncidentList
{
	/* Create XML Request */
	NSMutableString *xmlString = nil;
	if (historicList)
	{
		LTEntityDescriptor *entityDescriptor = self.entity.entityDescriptor;
		xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
		[xmlString appendString:@"<history>"];
		[xmlString appendString:[entityDescriptor xmlNodeString]];
		[xmlString appendFormat:@"<max_count>%i</max_count>", self.maxResultsCount];
		[xmlString appendString:@"</history>"];	
		self.debug = YES;
	}
	
	/* Refresh the incident list */
	LTCustomer *cust = self.customer;
	NSMutableURLRequest *theRequest;
	theRequest = [NSMutableURLRequest requestWithURL:[cust urlForXml:@"incident_list" timestamp:0]
										 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
									 timeoutInterval:60.0];
	
	/* Check for outbound data */
	if (xmlString && historicList)
	{
		/* Outbound XML doc to be sent */
		NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
		NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
		[theRequest addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
		[theRequest setHTTPMethod: @"POST"];
		NSMutableData *postData = [NSMutableData data];
		[postData appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
		[postData appendData:[@"Content-Disposition: form-data; name=\"xmlfile\"; filename=\"ltouch.xml\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
		[postData appendData:[@"Content-Type: text/xml\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
		[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
		[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
		[postData appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
		[postData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
		[theRequest setHTTPBody:postData];
	}
	
	/* Establish Connection */
	NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		refreshInProgress = YES;
		receivedData=[[NSMutableData data] retain];
		currentRequest = REQ_LIST;
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}	
}

#pragma mark Public API Methods

- (void) refreshCountOnly
{
	/* Refreshes the Incident Count only */

	refreshCountOnly = YES;
	
	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)[(LTEntity *)customer coreDeployment] enabled])
	{
		/* Customer disabled or refresh already in progress, do not proceed */
		return;
	}

	/* Request count and version */
	[self _requestCountAndVersion];
}

- (void) refresh
{
	/* If this is NOT a historic list then this function first 
	 * refreshes the Incident Version/Count and then if there 
	 * is a change in the version number it refreshes the entire list
	 */
	
	refreshCountOnly = NO;

	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)[(LTEntity *)customer coreDeployment] enabled])
	{
		/* Customer disabled or refresh already in progress, do not proceed */
		return;
	}

	/* Request count or list */
	if (historicList)
	{ 
		/* Historic list, proceed straight to full list download */
		[self _requestIncidentList]; 
	}
	else 
	{ 
		/* Acive list, update the count (and version) first */
		NSLog (@"[%@ refresh] requesting count and version", self);
		[self _requestCountAndVersion];
	}
}

#pragma mark Parsing

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
	/* Parse XML */
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	LCXMLParseOperation *xmlParser = [[LCXMLParseOperation new] autorelease];
	xmlParser.xmlData = receivedData;
	xmlParser.delegate = self;
	[appDelegate.operationQueue addOperation:xmlParser];
	
	/* Clean-up */
    [connection release];
	
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Check Thread */
	if ([NSThread currentThread] != [NSThread mainThread])
	{
		[NSException raise:@"LTEntity-parserDidFinish-IncorrectThread"
					format:@"An instance of LTEntity received a message to parserDidFinish on a thread that was NOT that main thread"];
	}
	
	/* Check for version info */
	BOOL listHasChanged = NO;
	if ([rootNode.properties objectForKey:@"version"]) 
	{ 
		/* Check the version number */
		unsigned int newVersion = (unsigned long) [[rootNode.properties objectForKey:@"version"] integerValue]; 
		if (newVersion != listVersion)
		{
			/* The list has changed */
			listHasChanged = YES;
			
			/* If this is a list request, the new version number can be set */
			if (currentRequest == REQ_LIST)
			{
				listVersion = newVersion;
			}
		}
			
		/* Update incident count */
		if ([rootNode.properties objectForKey:@"count"]) 
		{ 
			unsigned long newCount = (unsigned long) [[rootNode.properties objectForKey:@"count"] integerValue]; 
            NSLog (@"newCount is %i, incidentCount is %i", newCount, incidentCount);
			if (newCount != incidentCount)
			{
				incidentCount = newCount;
                NSLog (@"Dispatching LTIncidentListCountUpdated");
				[[NSNotificationCenter defaultCenter] postNotificationName:@"LTIncidentListCountUpdated" object:self];
			}
		}
	}
	
	/* Interpret Incident List*/
	if (currentRequest == REQ_LIST)
	{
		NSMutableArray *seenIncidents = [NSMutableArray array];
		for (LCXMLNode *childNode in rootNode.children)
		{ 
			if ([childNode.name isEqualToString:@"incident"])
			{
				LTIncident *curIncident = [incidentDict objectForKey:[childNode.properties objectForKey:@"id"]];
				if (!curIncident)
				{
					curIncident = [LTIncident new];
					curIncident.identifier = [[childNode.properties objectForKey:@"id"] intValue];
					if ([[childNode.properties objectForKey:@"start_sec"] intValue] > 0)
					{ curIncident.startDate = [NSDate dateWithTimeIntervalSince1970:[[childNode.properties objectForKey:@"start_sec"] doubleValue]]; }
					curIncident.raisedValue = [childNode.properties objectForKey:@"raised_valstr"];
					[incidents addObject:curIncident];
					[incidentDict setObject:curIncident forKey:[childNode.properties objectForKey:@"id"]];
				}
				[seenIncidents addObject:curIncident];
				if ([[childNode.properties objectForKey:@"end_sec"] intValue] > 0)
				{ curIncident.endDate = [NSDate dateWithTimeIntervalSince1970:[[childNode.properties objectForKey:@"end_sec"] doubleValue]]; }			
				curIncident.caseIdentifier = [[childNode.properties objectForKey:@"caseid"] intValue];
							
				for (LCXMLNode *incChildNode in childNode.children)
				{
					if ([incChildNode.name isEqualToString:@"action"])
					{
						LTAction *curAction = [curIncident.actionDict objectForKey:[incChildNode.properties objectForKey:@"id"]];
						if (!curAction)
						{
							curAction = [LTAction new];
							curAction.identifier = [[incChildNode.properties objectForKey:@"id"] intValue];
							curAction.desc = [incChildNode.properties objectForKey:@"desc"];
							curAction.incident = curIncident;
							curAction.scriptFile = [incChildNode.properties objectForKey:@"script_file"];
							curAction.enabled = [[incChildNode.properties objectForKey:@"enabled"] intValue];
							curAction.activationMode = [[incChildNode.properties objectForKey:@"activation"] intValue];
							curAction.delay = [[incChildNode.properties objectForKey:@"delay"] intValue];
							curAction.rerun = [[incChildNode.properties objectForKey:@"rerun"] intValue];
							curAction.rerunDelay = [[incChildNode.properties objectForKey:@"rerun_delay"] intValue];
							curAction.timeFiltered = [[incChildNode.properties objectForKey:@"time_filter"] intValue];
							curAction.dayMask = [[incChildNode.properties objectForKey:@"day_mask"] intValue];
							curAction.startHour = [[incChildNode.properties objectForKey:@"start_hour"] intValue];
							curAction.endHour = [[incChildNode.properties objectForKey:@"end_hour"] intValue];						
							[curIncident.actions addObject:curAction];
							[curIncident.actionDict setObject:curAction forKey:[incChildNode.properties objectForKey:@"id"]];
						}
						curAction.runCount = [[incChildNode.properties objectForKey:@"run_count"] intValue];
						curAction.runState = [[incChildNode.properties objectForKey:@"runstate"] intValue];
										}
					else if ([incChildNode.name isEqualToString:@"entity_descriptor"])
					{
						if (!curIncident.entityDescriptor)
						{ 
							/* Interpret entityDescriptor */
							curIncident.entityDescriptor = [LTEntityDescriptor entityDescriptorFromXml:incChildNode]; 
							curIncident.resourceAddress = curIncident.entityDescriptor.resourceAddress;
							
							/* Create stand-alone entity for the incident */
							curIncident.metric = [LTEntity new];
							curIncident.metric.type = 6;
							curIncident.metric.name = curIncident.entityDescriptor.metName;
							curIncident.metric.desc = curIncident.entityDescriptor.metDesc;
							curIncident.metric.username = [customer username];
							curIncident.metric.password = [customer password];
							curIncident.metric.customer = customer;
							curIncident.metric.customerName = curIncident.entityDescriptor.custName;
							curIncident.metric.resourceAddress = curIncident.resourceAddress;
							curIncident.metric.ipAddress = [customer ipAddress];
							curIncident.metric.coreDeployment = [customer coreDeployment];
							LTEntityDescriptor *metEntityDesc = [curIncident.entityDescriptor copy]; 
							metEntityDesc.type = 6;
							metEntityDesc.trgName = nil;
							metEntityDesc.trgDesc = nil;
							curIncident.metric.entityDescriptor = metEntityDesc;
							curIncident.metric.entityAddress = [curIncident.metric.entityDescriptor entityAddress];						
						}
						
					}
				}
				
			}
		}	
		
		/* Check obsolescence */
		NSMutableArray *obsoleteIncidents = [NSMutableArray array];
		for (LTIncident *inc in incidents)
		{
			if (![seenIncidents containsObject:inc])
			{ [obsoleteIncidents addObject:inc]; }
		}
		for (LTIncident *inc in obsoleteIncidents)
		{
			[incidents removeObject:inc];
			[incidentDict removeObjectForKey:[NSString stringWithFormat:@"%i", inc.identifier]];
		}
	}
	
	/* Clean-up */
    [receivedData release];
		
	/* Check to see if a follow-up refresh is needed */
	if (currentRequest == REQ_COUNTANDVERSION && listHasChanged && !refreshCountOnly)
	{
		/* This operation is not a count-only, and a change
		 * in the incident list has been detected by a the
		 * version number differing. Perform a full list load
		 */
		[self _requestIncidentList];
	}	
	else 
	{
		/* No follow-up needed, set State and Post Notification */
		self.refreshInProgress = NO;
		[[NSNotificationCenter defaultCenter] postNotificationName:@"LTIncidentListRefreshFinished" object:self];
	}
}

#pragma mark "Properties"

@synthesize incidents;
@synthesize incidentDict;
@synthesize historicList;
@synthesize maxResultsCount;
@synthesize entity;
- (void) setEntity:(LTEntity *)value
{
	[entity release];
	entity = [value retain];
	
	self.customer = entity.customer;
}
@synthesize incidentCount;

@end
