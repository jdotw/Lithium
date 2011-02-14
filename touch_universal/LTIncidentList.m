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
    self.entity = nil;  // nil the entity to release and remove observers, etc
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

- (void) _refresh
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
		[self _requestCountAndVersion];
	}    
}

- (void) refresh
{
    [self _refresh];
}

- (void) forceRefresh
{
    [self _refresh];
}

#pragma mark Parsing

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{    
    /* Parse XML Using TBXML */
    BOOL listHasChanged = NO;
    TBXML *xml = [TBXML tbxmlWithXMLData:receivedData];
    if (xml && xml.rootXMLElement)
    {
        /* Check for version info */
        if ([TBXML textForElementNamed:@"version" parentElement:xml.rootXMLElement]) 
        { 
            /* Check the version number */
            unsigned int newVersion = (unsigned long) [TBXML intFromTextForElementNamed:@"version" parentElement:xml.rootXMLElement]; 
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
            if ([TBXML textForElementNamed:@"count" parentElement:xml.rootXMLElement]) 
            { 
                unsigned long newCount = (unsigned long) [TBXML intFromTextForElementNamed:@"count" parentElement:xml.rootXMLElement]; 
                if (newCount != incidentCount)
                {
                    incidentCount = newCount;
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"LTIncidentListCountUpdated" object:self];
                }
            }
        }
	
        /* Interpret Incident List*/
        if (currentRequest == REQ_LIST)
        {
            NSMutableArray *seenIncidents = [NSMutableArray array];
            TBXMLElement *node = xml.rootXMLElement;
            for (node=node->firstChild; node; node = node->nextSibling)
            {
                NSString *nodeName = [TBXML elementName:node];
                if ([nodeName isEqualToString:@"incident"])
                {
                    LTIncident *curIncident = [incidentDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                    if (!curIncident)
                    {
                        curIncident = [[LTIncident new] autorelease];
                        curIncident.identifier = [TBXML intFromTextForElementNamed:@"id" parentElement:node];
                        if ([TBXML intFromTextForElementNamed:@"start_sec" parentElement:node] > 0)
                        { curIncident.startDate = [NSDate dateWithTimeIntervalSince1970:[[TBXML textForElementNamed:@"start_sec" 
                                                                                                      parentElement:node] doubleValue]]; }
                        curIncident.raisedValue = [TBXML textForElementNamed:@"raised_valstr" parentElement:node];
                        [incidents addObject:curIncident];
                        [incidentDict setObject:curIncident forKey:[TBXML textForElementNamed:@"id" parentElement:node]];
                    }
                    [seenIncidents addObject:curIncident];
                    if ([TBXML intFromTextForElementNamed:@"end_sec" parentElement:node] > 0)
                    { curIncident.endDate = [NSDate dateWithTimeIntervalSince1970:[[TBXML textForElementNamed:@"end_sec" 
                                                                                                parentElement:node] doubleValue]]; }
                    curIncident.caseIdentifier = [TBXML intFromTextForElementNamed:@"caseid" parentElement:node];
                                
                    TBXMLElement *incChildNode;
                    for (incChildNode = node->firstChild; incChildNode; incChildNode=incChildNode->nextSibling)
                    {
                        NSString *childNodeName = [TBXML elementName:incChildNode];
                        if ([childNodeName isEqualToString:@"action"])
                        {
                            LTAction *curAction = [curIncident.actionDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:incChildNode]];
                            if (!curAction)
                            {
                                curAction = [[LTAction new] autorelease];
                                curAction.identifier = [TBXML intFromTextForElementNamed:@"id" parentElement:incChildNode];
                                curAction.desc = [TBXML textForElementNamed:@"desc" parentElement:incChildNode];
                                curAction.incident = curIncident;
                                curAction.scriptFile = [TBXML textForElementNamed:@"script_file" parentElement:incChildNode];
                                curAction.enabled = [TBXML intFromTextForElementNamed:@"enabled" parentElement:incChildNode];
                                curAction.activationMode = [TBXML intFromTextForElementNamed:@"activation" parentElement:incChildNode];
                                curAction.delay = [TBXML intFromTextForElementNamed:@"delay" parentElement:incChildNode];
                                curAction.rerun = [TBXML intFromTextForElementNamed:@"rerun" parentElement:incChildNode];
                                curAction.rerunDelay = [TBXML intFromTextForElementNamed:@"rerun_delay" parentElement:incChildNode];
                                curAction.timeFiltered = [TBXML intFromTextForElementNamed:@"time_filter" parentElement:incChildNode];
                                curAction.dayMask = [TBXML intFromTextForElementNamed:@"day_mask" parentElement:incChildNode];
                                curAction.startHour = [TBXML intFromTextForElementNamed:@"start_hour" parentElement:incChildNode];
                                curAction.endHour = [TBXML intFromTextForElementNamed:@"end_hour" parentElement:incChildNode];
                                [curIncident.actions addObject:curAction];
                                [curIncident.actionDict setObject:curAction forKey:[TBXML textForElementNamed:@"id" parentElement:incChildNode]];
                            }
                            curAction.runCount = [TBXML intFromTextForElementNamed:@"run_count" parentElement:incChildNode];
                            curAction.runState = [TBXML intFromTextForElementNamed:@"runstate" parentElement:incChildNode];
                        }
                        else if ([childNodeName isEqualToString:@"entity_descriptor"])
                        {
                            if (!curIncident.entityDescriptor)
                            { 
                                /* Interpret entityDescriptor */
                                curIncident.entityDescriptor = [LTEntityDescriptor entityDescriptorFromXml:incChildNode]; 
                                curIncident.resourceAddress = curIncident.entityDescriptor.resourceAddress;
                                
                                /* Create stand-alone entity for the incident */
                                curIncident.metric = [[LTEntity new] autorelease];
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
                                LTEntityDescriptor *metEntityDesc = [[curIncident.entityDescriptor copy] autorelease];
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
    }
    
	/* Clean-up */
    [receivedData release];
    [connection release];
		
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
    
    /* Set last refresh date */
    if (currentRequest == REQ_LIST)
    {
        self.lastRefresh = [NSDate date];
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
@synthesize lastRefresh;

@end
