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

#pragma mark "Refresh"

- (void) refresh
{
	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)[(LTEntity *)customer coreDeployment] enabled])
	{
		return;
	}
	
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
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[cust urlForXml:@"incident_list" timestamp:0]
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
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}
}

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
	
	/* Interpret */
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
						
						NSLog (@"Incident entityDesc type is %@", curIncident.entityDescriptor.entityAddress);

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
	
	/* Clean-up */
    [receivedData release];
	
	/* Post Notification */
	self.refreshInProgress = NO;
	[[NSNotificationCenter defaultCenter] postNotificationName:@"IncidentListRefreshFinished" object:self];
}


//- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict 
//{
//	curXmlString = [[NSMutableString alloc] init];
//	
//	if ([elementName isEqualToString:@"incident"])
//	{
//		curIncident = [LTIncident new];
//		[incidents addObject:curIncident];
//		[curIncident autorelease];
//	}
//	if ([elementName isEqualToString:@"action"])
//	{
//		curAction = [LTAction new];
//		curAction.incident = curIncident;
//		[curIncident.actions addObject:curAction];
//		[curAction autorelease];
//	}
//	else if ([elementName isEqualToString:@"entity_descriptor"])
//	{
//		curIncident.entityDescriptor = [LTEntityDescriptor new];
//	}
//}

//- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
//{
//	[curXmlString appendString:string];
//}

//- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName 
//{
//	/* Incident */
//	if ([elementName isEqualToString:@"incident"])
//	{ curIncident = nil; }
//	else if ([elementName isEqualToString:@"id"] && !curAction)
//	{ curIncident.identifier = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"start_sec"])
//	{ curIncident.startDate = [NSDate dateWithTimeIntervalSince1970:[curXmlString doubleValue]]; }
//	else if ([elementName isEqualToString:@"end_sec"])
//	{
//		if (![curXmlString isEqualToString:@"0"])
//		{ curIncident.endDate = [NSDate dateWithTimeIntervalSince1970:[curXmlString doubleValue]]; }
//	}
//	else if ([elementName isEqualToString:@"caseid"])
//	{ curIncident.caseIdentifier = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"raised_valstr"])
//	{ curIncident.raisedValue = curXmlString; }
//	
//	/* Action */
//	else if ([elementName isEqualToString:@"action"])
//	{ curAction = nil; }
//	else if ([elementName isEqualToString:@"id"] && curAction)
//	{ curAction.identifier = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"desc"] && curAction) 
//	{ curAction.desc = curXmlString; }
//	else if ([elementName isEqualToString:@"enabled"] && curAction) 
//	{ curAction.enabled = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"activation"] && curAction) 
//	{ curAction.activationMode = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"delay"] && curAction) 
//	{ curAction.delay = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"rerun"] && curAction) 
//	{ curAction.rerun = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"rerun_delay"] && curAction) 
//	{ curAction.rerunDelay = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"time_filter"] && curAction) 
//	{ curAction.timeFiltered = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"day_mask"] && curAction) 
//	{ curAction.dayMask = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"start_hour"] && curAction) 
//	{ curAction.startHour = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"end_hour"] && curAction) 
//	{ curAction.endHour = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"run_count"] && curAction) 
//	{ curAction.runCount = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"runstate"] && curAction) 
//	{ curAction.runState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"script_file"] && curAction) 
//	{ curAction.scriptFile = curXmlString; }
//	
//	/* Entity Descriptor */
//	else if ([elementName isEqualToString:@"name"]) 
//	{ curIncident.entityDescriptor.name = curXmlString; }
//	else if ([elementName isEqualToString:@"desc"] && !curAction) 
//	{ curIncident.entityDescriptor.desc = curXmlString; }
//	else if ([elementName isEqualToString:@"opstate_num"]) 
//	{ curIncident.entityDescriptor.opState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"adminstate_num"]) 
//	{ curIncident.entityDescriptor.adminState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"cust_name"]) 
//	{ curIncident.entityDescriptor.custName = curXmlString; }
//	else if ([elementName isEqualToString:@"cust_desc"]) 
//	{ curIncident.entityDescriptor.custDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"cust_opstate_num"]) 
//	{ curIncident.entityDescriptor.custOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"cust_adminstate_num"]) 
//	{ curIncident.entityDescriptor.custAdminState = [curXmlString intValue]; }
//
//	else if ([elementName isEqualToString:@"site_name"]) 
//	{ curIncident.entityDescriptor.siteName = curXmlString; }
//	else if ([elementName isEqualToString:@"site_desc"]) 
//	{ curIncident.entityDescriptor.siteDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"site_suburb"]) 
//	{ curIncident.entityDescriptor.siteSuburb = curXmlString; }
//	else if ([elementName isEqualToString:@"site_opstate_num"]) 
//	{ curIncident.entityDescriptor.siteOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"site_adminstate_num"]) 
//	{ curIncident.entityDescriptor.siteAdminState = [curXmlString intValue]; }
//	
//	else if ([elementName isEqualToString:@"dev_name"]) 
//	{ curIncident.entityDescriptor.devName = curXmlString; }
//	else if ([elementName isEqualToString:@"dev_desc"]) 
//	{ curIncident.entityDescriptor.devDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"dev_opstate_num"]) 
//	{ curIncident.entityDescriptor.devOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"dev_adminstate_num"]) 
//	{ curIncident.entityDescriptor.devAdminState = [curXmlString intValue]; }
//	
//	else if ([elementName isEqualToString:@"cnt_name"]) 
//	{ curIncident.entityDescriptor.cntName = curXmlString; }
//	else if ([elementName isEqualToString:@"cnt_desc"]) 
//	{ curIncident.entityDescriptor.cntDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"cnt_opstate_num"]) 
//	{ curIncident.entityDescriptor.cntOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"cnt_adminstate_num"]) 
//	{ curIncident.entityDescriptor.cntAdminState = [curXmlString intValue]; }
//	
//	else if ([elementName isEqualToString:@"obj_name"]) 
//	{ curIncident.entityDescriptor.objName = curXmlString; }
//	else if ([elementName isEqualToString:@"obj_desc"]) 
//	{ curIncident.entityDescriptor.objDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"obj_opstate_num"]) 
//	{ curIncident.entityDescriptor.objOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"obj_adminstate_num"]) 
//	{ curIncident.entityDescriptor.objAdminState = [curXmlString intValue]; }
//	
//	else if ([elementName isEqualToString:@"met_name"]) 
//	{ curIncident.entityDescriptor.metName = curXmlString; }
//	else if ([elementName isEqualToString:@"met_desc"]) 
//	{ curIncident.entityDescriptor.metDesc = curXmlString; }
//	else if ([elementName isEqualToString:@"met_opstate_num"]) 
//	{ curIncident.entityDescriptor.metOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"met_adminstate_num"]) 
//	{ curIncident.entityDescriptor.metAdminState = [curXmlString intValue]; }	
//	
//	else if ([elementName isEqualToString:@"trg_name"]) 
//	{ curIncident.entityDescriptor.trgName = curXmlString; }
//	else if ([elementName isEqualToString:@"trg_desc"]) 
//	{ curIncident.entityDescriptor.trgDesc = curXmlString; }	
//	else if ([elementName isEqualToString:@"trg_opstate_num"]) 
//	{ curIncident.entityDescriptor.trgOpState = [curXmlString intValue]; }
//	else if ([elementName isEqualToString:@"trg_adminstate_num"]) 
//	{ curIncident.entityDescriptor.trgAdminState = [curXmlString intValue]; }
//	
//	else if ([elementName isEqualToString:@"resaddr"]) 
//	{ curIncident.resourceAddress = curXmlString; }
//	else if ([elementName isEqualToString:@"entity_descriptor"])
//	{
//		/* Create stand-alone entity for the incident */
//		curIncident.metric = [LTEntity new];
//		curIncident.metric.type = 6;
//		curIncident.metric.name = curIncident.entityDescriptor.metName;
//		curIncident.metric.desc = curIncident.entityDescriptor.metDesc;
//		curIncident.metric.username = [customer username];
//		curIncident.metric.password = [customer password];
//		curIncident.metric.customer = customer;
//		curIncident.metric.customerName = curIncident.entityDescriptor.custName;
//		curIncident.metric.resourceAddress = curIncident.resourceAddress;
//		curIncident.metric.ipAddress = [customer ipAddress];
//		curIncident.metric.coreDeployment = [customer coreDeployment];
//		LTEntityDescriptor *metEntityDesc = [curIncident.entityDescriptor copy];
//		metEntityDesc.type = 6;
//		metEntityDesc.trgName = nil;
//		metEntityDesc.trgDesc = nil;
//		curIncident.metric.entityDescriptor = metEntityDesc;
//		curIncident.metric.entityAddress = [curIncident.metric.entityDescriptor entityAddress];
//	}
//	
//	[curXmlString release];
//	curXmlString = nil;
//}

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

@end
