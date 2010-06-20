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
#import "LithiumAppDelegate.h"

@implementation LTMetricGraphRequest

- (LTMetricGraphRequest *) init
{
	[super init];
	
	self.size = CGSizeMake (320.0f, 93.0f);
	self.referenceDate = [NSDate date];
	
	return self;
}

- (void) dealloc
{
	[imageData release];
	[graphInfo release];
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
	NSString *str = [NSString stringWithFormat:@"%f", [referenceDate timeIntervalSince1970]];
	NSMutableString *xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<graph>"];
	[xmlString appendString:[entityDescriptor xmlNodeString]];
	[xmlString appendFormat:@"<ref_sec>%@</ref_sec>", str];
	[xmlString appendFormat:@"<period>%@</period>", @"1"];
	[xmlString appendFormat:@"<format>%@</format>", @"PNG"];
	[xmlString appendFormat:@"<width>%@</width>", [NSString stringWithFormat:@"%.0f", size.width]];
	[xmlString appendFormat:@"<height>%@</height>", [NSString stringWithFormat:@"%.0f", size.height]];
	[xmlString appendString:@"</graph>"];	
	
	/* Refresh the incident list */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[metric urlForXml:@"xmlgraph_render" timestamp:0]
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
	
	refreshStage = 1;
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
	if (refreshStage == 1)
	{
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
		
		/* Retrieve the PDF */
		refreshStage = 2;
		NSString *urlString = [NSString stringWithFormat:@"%@/image_cache/%@", [metric urlPrefix], imageFile];
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
		}	
	}
	else if (refreshStage == 2)
	{
		/* Process PDF */
		self.imageData = receivedData;

		/* Post Notification */
		refreshInProgress = NO;
		[[NSNotificationCenter defaultCenter] postNotificationName:@"GraphRefreshFinished" object:self];
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
@synthesize metric;
- (void) setMetric:(LTEntity *)value
{
	[metric release];
	metric = [value retain];
	self.customer = metric.customer;
}
@synthesize imageData;
@synthesize minValue;
@synthesize maxValue;

@end
