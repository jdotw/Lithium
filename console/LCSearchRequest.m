//
//  LCSearchRequest.m
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCSearchRequest.h"

#import "LCEntityDescriptor.h"
#import "LCMetric.h"
#import "LCMetricValue.h"

@implementation LCSearchRequest

#pragma mark "Constructors"

- (id) init
{
	self = [super init];
	if (self != nil) {
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
	[entity release];
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
	xmlRequest = [[LCXMLRequest requestWithCriteria:entity.customer
										   resource:[entity resourceAddress]
											 entity:[entity entityAddress]
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
	NSMutableArray *seenEntities = [NSMutableArray array];
	for (LCXMLNode *resultNode in rootNode.children)
	{
		LCEntity *resultEntity = nil;
		for (LCXMLNode *childNode in resultNode.children)
		{
			if ([childNode.name isEqualToString:@"entity_descriptor"])
			{
				LCEntityDescriptor *entDesc = [LCEntityDescriptor descriptorWithXmlNode:childNode];
				resultEntity = [entDesc locateEntity:YES];
			
				if (![resultDict objectForKey:[entDesc addressString]])
				{ [self insertObject:resultEntity inResultsAtIndex:results.count]; }
			
				[seenEntities addObject:resultEntity];
			}
			else if ([childNode.name isEqualToString:@"value"] && entity.type == 6)
			{
				LCMetric *metric = (LCMetric *) entity;
				LCMetricValue *value = [[LCMetricValue alloc] initWithXmlNode:childNode];
				LCMetricValue *latestValue = [metric.metricValues objectAtIndex:0];
				if (value.timestamp > latestValue.timestamp)
				{
					/* Use received value */
					
				}
				[value autorelease];
			}
		}
	}
	
	/* Check for obsolete entities */
	NSMutableArray *removeEntities = [NSMutableArray array];
	for (LCEntity *localEntity in results)
	{
		if (![seenEntities containsObject:localEntity])
		{ [removeEntities addObject:localEntity]; }
	}
	for (LCEntity *localEntity in removeEntities)
	{
		[self removeObjectFromResultsAtIndex:[results indexOfObject:localEntity]];
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

@synthesize entity;
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
- (void) insertObject:(LCEntity *)newEntity inResultsAtIndex:(unsigned int)index
{
	[results insertObject:newEntity atIndex:index];
	[resultDict setObject:newEntity forKey:[newEntity.entityAddress addressString]];
}
- (void) removeObjectFromResultsAtIndex:(unsigned int)index
{
	LCEntity *removeEntity = [results objectAtIndex:index];
	[results removeObjectAtIndex:index];
	[resultDict removeObjectForKey:[removeEntity.entityAddress addressString]];
}

@end
