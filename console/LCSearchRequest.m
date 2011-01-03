//
//  LCSearchRequest.m
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCSearchRequest.h"

#import "LCCustomer.h"
#import "LCXMLRequest.h"
#import "LCResourceAddress.h"
#import "LCEntityDescriptor.h"
#import "LCMetric.h"
#import "LCMetricValue.h"
#import "LCSearchResult.h"

@implementation LCSearchRequest

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (self != nil) 
	{
		results = [[NSMutableArray array] retain];
		resultDict = [[NSMutableDictionary dictionary] retain];
	}
	return self;
}
	

- (void) dealloc
{
	if (xmlRequest)
	{
		[xmlRequest cancel];
		[xmlRequest release];
	}
	[customer release];
	[keywords release];
	[searchString release];
	[results release];
	[resultDict release];
	[super dealloc];
}

#pragma mark "XML Operations"

- (void) search
{
	/* Create XML */
	NSXMLElement *rootnode = (NSXMLElement *) [NSXMLNode elementWithName:@"criteria"];
	NSXMLDocument *xmldoc = [NSXMLDocument documentWithRootElement:rootnode];	
	[xmldoc setVersion:@"1.0"];
	[xmldoc setCharacterEncoding:@"UTF-8"];
	for (NSString *keyword in keywords)
	{
		[rootnode addChild:[NSXMLNode elementWithName:@"keyword" stringValue:keyword]];
	}	
	[rootnode addChild:[NSXMLNode elementWithName:@"operator" stringValue:[NSString stringWithFormat:@"%i", searchOperator]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"regex" stringValue:[NSString stringWithFormat:@"%i", useRegex]]];
	[rootnode addChild:[NSXMLNode elementWithName:@"max_type" stringValue:[NSString stringWithFormat:@"%i", maximumType]]];
	
	/* Create and perform request */
	xmlRequest = [[LCXMLRequest requestWithCriteria:customer
										   resource:[customer resourceAddress]
											 entity:[customer entityAddress]
											xmlname:@"search"
											 refsec:0
											 xmlout:xmldoc] retain];
	[xmlRequest setDelegate:self];
	[xmlRequest setThreadedXmlDelegate:self];
	[xmlRequest setPriority:XMLREQ_PRIO_HIGH];
	
	[xmlRequest performAsyncRequest];
	
	self.searchInProgress = YES;	
}

#pragma mark "XML Parser Delegate Methods"

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	/* Process results */
	NSMutableDictionary *seenResultDict = [NSMutableDictionary dictionary];
	for (LCXMLNode *resultNode in rootNode.children)
	{
		LCSearchResult *result = [[LCSearchResult new] autorelease];
		
		result.entityType = [[resultNode.properties objectForKey:@"ent_type"] intValue];
		result.entityAddress = [LCEntityAddress addressWithString:[resultNode.properties objectForKey:@"ent_addr"]];
		result.resourceAddress = [LCResourceAddress addressWithString:[resultNode.properties objectForKey:@"res_addr"]];
		result.desc = [resultNode.properties objectForKey:@"desc"];
		result.custDesc = [resultNode.properties objectForKey:@"cust_desc"];
		result.siteDesc = [resultNode.properties objectForKey:@"site_desc"];
		result.devDesc = [resultNode.properties objectForKey:@"dev_desc"];
		result.cntDesc = [resultNode.properties objectForKey:@"cnt_desc"];
		result.objDesc = [resultNode.properties objectForKey:@"obj_desc"];
		
		[self insertObject:result inResultsAtIndex:results.count];
		[seenResultDict setObject:result forKey:[result.entityAddress addressString]];
	}
	
	/* Check for obsolete entities */
	NSMutableArray *obsoleteResults = [NSMutableArray array];
	for (LCSearchResult *result in results)
	{
		if (![seenResultDict objectForKey:[result.entityAddress addressString]])
		{ [obsoleteResults addObject:result]; }
	}
	for (LCSearchResult *result in obsoleteResults)
	{
		[self removeObjectFromResultsAtIndex:[results indexOfObject:result]];
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	if (sender == xmlRequest)
	{
		[xmlRequest release];
		xmlRequest = nil;
	}
	
	self.searchInProgress = NO;
}

#pragma mark "Properties"

@synthesize customer;
@synthesize keywords;
@synthesize searchOperator;
@synthesize useRegex;
@synthesize maximumType;

@synthesize searchString;
- (void) setSearchString:(NSString *)value
{
	[searchString release];
	searchString = [value copy];
	
	self.keywords = [searchString componentsSeparatedByString:@" "];
}

@synthesize searchInProgress;

@synthesize results;
@synthesize resultDict;
- (void) insertObject:(LCSearchResult *)result inResultsAtIndex:(unsigned int)index
{
	[results insertObject:result atIndex:index];
	[resultDict setObject:result forKey:[result.entityAddress addressString]];
}
- (void) removeObjectFromResultsAtIndex:(unsigned int)index
{
	LCSearchResult *result = [results objectAtIndex:index];
	[results removeObjectAtIndex:index];
	[resultDict removeObjectForKey:[result.entityAddress addressString]];
}

@end
