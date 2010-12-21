//
//  LTMetricGraphRequest.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricGraphRequest.h"

#import "LTCustomer.h"
#import "LTCoreDeployment.h"
#import "AppDelegate.h"

@implementation LTMetricGraphRequest

- (LTMetricGraphRequest *) init
{
	[super init];
	
	self.size = CGSizeMake (320.0f, 93.0f);
	self.referenceDate = [NSDate date];
	metrics = [[NSMutableArray array] retain];
	
	return self;
}

- (void) dealloc
{
	[metrics release];
	[imageData release];
	[graphInfo release];
	[super dealloc];
}

#pragma mark "Refresh"

- (void) refresh
{
	AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
	if (![[appDelegate.operationQueue operations] containsObject:self])
	{
		[appDelegate.operationQueue addOperation:self];
	}
	refreshInProgress = YES;
}

- (void) main
{
	/* Check state */
	if (![(LTCoreDeployment *)customer.coreDeployment enabled])
	{
		return;
	}
	
	/* Create XML Request */
	NSString *refSecStr = [NSString stringWithFormat:@"%f", [referenceDate timeIntervalSince1970]];
	NSMutableString *xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<graph>"];
	for (LTEntity *metric in metrics)
	{
		LTEntityDescriptor *entityDescriptor = metric.entityDescriptor;
		[xmlString appendString:[entityDescriptor xmlNodeString]];
	}
	if (startSec != 0 && endSec != 0)
	{
		/* Custom period render */
		[xmlString appendFormat:@"<start_sec>%i</start_sec>", startSec];
		[xmlString appendFormat:@"<end_sec>%i</end_sec>", endSec];
		[xmlString appendFormat:@"<period>%@</period>", @"6"];				
	}
	else
	{
		/* Default */
		[xmlString appendFormat:@"<ref_sec>%@</ref_sec>", refSecStr];
		[xmlString appendFormat:@"<period>%@</period>", @"1"];		
	}

	[xmlString appendFormat:@"<format>%@</format>", @"PDF"];
	[xmlString appendFormat:@"<width>%@</width>", [NSString stringWithFormat:@"%.0f", size.width]];
	[xmlString appendFormat:@"<height>%@</height>", [NSString stringWithFormat:@"%.0f", size.height]];
	if (allWhiteLines) [xmlString appendFormat:@"<all_white>%@</all_white>", @"1"];
	[xmlString appendString:@"</graph>"];	
	
	/* Refresh the incident list */
	urlReq = [NSMutableURLRequest requestWithURL:[self.metric urlForXml:@"xmlgraph_render" timestamp:0]
									 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
								 timeoutInterval:60.0];

	/* Outbound XML doc to be sent */
	NSString *formBoundary = [[NSProcessInfo processInfo] globallyUniqueString];
	NSString *boundaryString = [NSString stringWithFormat: @"multipart/form-data; boundary=%@", formBoundary];
	[urlReq addValue:boundaryString forHTTPHeaderField:@"Content-Type"];
	[urlReq setHTTPMethod: @"POST"];
	NSMutableData *postData = [NSMutableData data];
	[postData appendData:[[NSString stringWithFormat:@"--%@\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Disposition: form-data; name=\"xmlfile\"; filename=\"ltouch.xml\"\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[@"Content-Type: text/xml\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[postData appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[urlReq setHTTPBody:postData];
	
	refreshStage = 1;
	receivedData=[[NSMutableData data] retain];
	
	[super main];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	if (refreshStage == 1)
	{
		/* Logging */
		if (debug)
		{
			NSLog (@"First stage received %@", [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding]);
		}
		
		/* Parse XML */
		NSXMLParser *parser = [[NSXMLParser alloc] initWithData:receivedData];
		[parser setShouldResolveExternalEntities:NO];
		[parser setDelegate:self];
		[parser parse];
		
		/* Clean-up */
		[parser release];
		[curXmlString release];
//		[connection release];
		[receivedData release];
		
		/* Retrieve the PDF */
		refreshStage = 2;
		NSString *urlString = [NSString stringWithFormat:@"%@/image_cache/%@", [self.metric urlPrefix], imageFile];
		if (debug) NSLog (@"%@ fetching actual image from %@", self, urlString);
		NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
																  cachePolicy:NSURLRequestReloadIgnoringCacheData
															  timeoutInterval:30.0];
		NSURLConnection *urlConn = [[NSURLConnection connectionWithRequest:urlRequest delegate:self] retain];
		if (urlConn)
		{
			/* Set up data */
			receivedData = [[NSMutableData data] retain];
		}
		else 
		{
			/* FIX */
			NSLog (@"ERROR: Failed to download image file %@", imageFile);
			finished = YES;
		}	
	}
	else if (refreshStage == 2)
	{
		/* Logging */
		if (debug)
		{
			NSLog (@"Second stage received %@", [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding]);
		}

		/* Process PDF */
		self.imageData = receivedData;

		/* Set Status */
		refreshInProgress = NO;
		finished = YES;
	}
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict 
{
	curXmlString = [[NSMutableString alloc] init];
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[curXmlString appendString:string];
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName 
{
	if ([elementName isEqualToString:@"imagefile"])
	{ imageFile = [curXmlString copy]; }
	else if ([elementName isEqualToString:@"output"])
	{ 
		/* Parse output */
		graphInfo = [curXmlString copy]; 
		NSArray *pairs = [graphInfo componentsSeparatedByString:@","];
		for (NSString *pair in pairs)
		{
			NSArray *words = [pair componentsSeparatedByString:@" "];
			if ([[words objectAtIndex:0] isEqualToString:@"value_min"])
			{ minValue = [[words objectAtIndex:2] floatValue]; }
			else if ([[words objectAtIndex:0] isEqualToString:@"value_max"])
			{ maxValue = [[words objectAtIndex:2] floatValue]; }
		}
	}
	
	[curXmlString release];
	curXmlString = nil;
}


#pragma mark "Properties"

@synthesize size;
@synthesize referenceDate;
- (LTEntity *)metric
{
	if (metrics.count > 0)
	{ return [metrics objectAtIndex:0]; }
	else
	{ return nil; }
}
- (void) setMetric:(LTEntity *)value
{
	self.customer = value.customer;
	[metrics removeAllObjects];
	[metrics addObject:value];
}
@synthesize imageData;
@synthesize minValue;
@synthesize maxValue;
@synthesize startSec;
@synthesize endSec;
@synthesize synchronous;
@synthesize metrics;
@synthesize rectToInvalidate;
@synthesize allWhiteLines;

@end
