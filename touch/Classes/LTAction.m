//
//  LTAction.m
//  Lithium
//
//  Created by James Wilson on 1/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTAction.h"

#import "LithiumAppDelegate.h"
#import "LTCustomer.h"

@implementation LTAction

#pragma mark "Constructors"

- (void) dealloc
{
	[desc release];
	[scriptFile release];
	[xmlOperationMessage release];
	[super dealloc];
}

#pragma mark "Execution"

- (void) performXmlOperation:(NSString *)xmlName
{
	/* Create XML Request */
	NSMutableString *xmlString;
	xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<action>"];
	[xmlString appendFormat:@"<incid>%i</incid>", self.incident.identifier];
	[xmlString appendFormat:@"<actionid>%i</actionid>", self.identifier];
	[xmlString appendString:@"</action>"];
	
	/* Refresh the incident list */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[self.incident.metric.customer urlForXml:xmlName timestamp:0]
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
	
	/* Establish Connection */
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		xmlOperationInProgress = YES;
		receivedData=[[NSMutableData data] retain];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}	
	
	/* Hold on to ourselves, just in case the list is refreshed */
	[self retain];
}

- (void) execute 
{ 
	[self performXmlOperation:@"action_execute"];
}

- (void) cancel
{
	[self performXmlOperation:@"action_cancelpending"];
}	

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	[super connection:connection didFailWithError:error];
	
	xmlOperationInProgress = NO;
	xmlOperationResult = 2;
	xmlOperationMessage = @"Error in communication with Lithium Core";
	[[NSNotificationCenter defaultCenter] postNotificationName:@"ActionExecutionFinished" object:self];

	[self release];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
	/* Set state */
	xmlOperationInProgress = NO;	
	
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
	receivedData = nil;
	
	/* Post Notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"ActionExecutionFinished" object:self];

	/* Release self */
	[self release];
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict 
{
	[curXmlString release];
	curXmlString = [[NSMutableString alloc] init];
}

- (void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string 
{
	[curXmlString appendString:string];
}

- (void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName 
{
	/* Status */
	if ([elementName isEqualToString:@"error"])
	{ 
		self.xmlOperationMessage = curXmlString;
		self.xmlOperationResult = 2;
	}
	else if ([elementName isEqualToString:@"message"])
	{
		self.xmlOperationMessage = curXmlString;
		self.xmlOperationResult = 1;
	}
	
	[curXmlString release];
	curXmlString = nil;
}

#pragma mark "Properties"

@synthesize identifier;
@synthesize desc;
@synthesize enabled;
@synthesize activationMode;
@synthesize delay;
@synthesize rerun;	
@synthesize rerunDelay;
@synthesize timeFiltered;
@synthesize dayMask;
@synthesize startHour;
@synthesize endHour;
@synthesize runCount;
@synthesize runState;
@synthesize scriptFile;
@synthesize incident;
- (void) setIncident:(LTIncident *)value
{
	[incident release];
	incident = [value retain];
	self.customer = incident.metric.customer;
}
@synthesize xmlOperationMessage;
@synthesize xmlOperationResult;

@end
