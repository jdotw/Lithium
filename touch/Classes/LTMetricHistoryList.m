//
//  LTMetricHistoryList.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricHistoryList.h"

#import "LTCoreDeployment.h"
#import "LTEntityDescriptor.h"
#import "LithiumAppDelegate.h"

@implementation LTMetricHistoryList

#pragma mark "Constructors"

- (LTMetricHistoryList *) init
{
	[super init];
	
	values = [[NSMutableArray array] retain];
	
	return self;
}

- (void) dealloc
{
	[values release];
	[super dealloc];
}

#pragma mark "Refresh"

- (void) refresh
{
	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)[metric coreDeployment] enabled])
	{
		return;
	}
	
	/* Create XML Request */
	LTEntityDescriptor *entityDescriptor = self.metric.entityDescriptor;
	NSString *str = [NSString stringWithFormat:@"%f", [[NSDate dateWithTimeIntervalSinceNow:0.0] timeIntervalSince1970]];
	NSMutableString *xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<history>"];
	[xmlString appendString:[entityDescriptor xmlNodeString]];
	[xmlString appendFormat:@"<ref_sec>%@</ref_sec>", str];
	[xmlString appendFormat:@"<period>%@</period>", @"1"];
	[xmlString appendString:@"</history>"];	
	
	/* Refresh the incident list */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[metric urlForXml:@"rrdxport" timestamp:0]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
	
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
	
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
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
	/* Clear Old List */
	[values removeAllObjects];
	refreshInProgress = NO;	
	
	/* Parse XML */
	NSXMLParser *parser = [[NSXMLParser alloc] initWithData:receivedData];
	[parser setShouldResolveExternalEntities:NO];
	[parser setDelegate:self];
	[parser parse];
	
	/* Clean-up */
	[parser release];
	[curXmlString release];
    [connection release];
    [receivedData release];
	
	/* Calculate min/avg/max */
	BOOL minValueSet = NO;
	self.minValue = 0.0f;
	self.avgValue = 0.0f;
	self.maxValue = 0.0f;
	int avgCount = 0;
	for (LTMetricValue *value in values)
	{
		if (![[NSString stringWithFormat:@"%f", value.minValue] isEqualToString:@"nan"] && (value.minValue < self.minValue || !minValueSet))
		{ 
			self.minValue = value.minValue; 
			minValueSet = YES;
		}
		if (![[NSString stringWithFormat:@"%f", value.avgValue] isEqualToString:@"nan"]) 
		{
			self.avgValue += value.avgValue;
			avgCount++;
		}
		if (![[NSString stringWithFormat:@"%f", value.maxValue] isEqualToString:@"nan"] && value.maxValue > self.maxValue)
		{ 
			self.maxValue = value.maxValue; 
		}
	}
	self.avgValue = self.avgValue / (float) avgCount;

	/* Post Notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"HistoryListRefreshFinished" object:self];
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict 
{
	curXmlString = [[NSMutableString alloc] init];
	
	if ([elementName isEqualToString:@"row"])
	{
		curValue = [LTMetricValue new];
		[values addObject:curValue];
		[curValue autorelease];
		valueIndex = 0;
	}
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[curXmlString appendString:string];
	
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName 
{
	if ([elementName isEqualToString:@"row"])
	{
		curValue = nil;
	}
	else if ([elementName isEqualToString:@"t"])
	{
		curValue.timestamp = [NSDate dateWithTimeIntervalSince1970:[curXmlString floatValue]];
	}
	else if ([elementName isEqualToString:@"v"])
	{
		switch (valueIndex)
		{
			case 0:
				if ([curXmlString isEqualToString:@"NaN"])
				{ curValue.minValue = NAN; }
				else
				{ 
					curValue.minValue = [curXmlString floatValue]; 
					if (!self.hasRealData) self.hasRealData = YES;
				}
				break;
			case 1:
				if ([curXmlString isEqualToString:@"NaN"])
				{ curValue.avgValue = NAN; }
				else
				{ 
					curValue.avgValue = [curXmlString floatValue]; 
					if (!self.hasRealData) self.hasRealData = YES;
				}
				break;
			case 2:
				if ([curXmlString isEqualToString:@"NaN"])
				{ curValue.maxValue = NAN; }
				else
				{ 
					curValue.maxValue = [curXmlString floatValue]; 
					if (!self.hasRealData) self.hasRealData = YES;
				}
				break;
		}	
		valueIndex++;
	}
	[curXmlString release];
	curXmlString = nil;
}

#pragma mark "Properties"

@synthesize values;
@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
	[metric release];
	metric = [value retain];
	self.customer = metric.customer;
}
@synthesize minValue;
@synthesize avgValue;
@synthesize maxValue;
@synthesize hasRealData;

@end
