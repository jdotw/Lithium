//
//  LTIncident.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTIncident.h"
#import "LTCustomer.h"

@implementation LTIncident

- (LTIncident *) init
{
	self = [super init];
	if (self)
	{
		actions = [[NSMutableArray array] retain];
		actionDict = [[NSMutableDictionary dictionary] retain];
	}
	return self;
}

- (void) dealloc
{
	[startDate release];
	[endDate release];
	[raisedValue release];
	[resourceAddress release];
	[entityDescriptor release];
	[metric release];
	[actions release];
	[actionDict release];
	[super dealloc];
}

#pragma mark -
#pragma mark Clearing

- (void) clear
{
	/* Clear the incident */
	NSString *urlString = [NSString stringWithFormat:@"%@/xml.php?action=xml_get&resaddr=%@&entaddr=%@&xmlname=incident_clear&refsec=0", 
						   [self.metric urlPrefix], self.metric.device.resourceAddress, self.entityDescriptor.entityAddress];
	NSLog (@"URL is %@", urlString);
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlString]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
	
	/* Establish Connection */
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		receivedData=[[NSMutableData data] retain];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download console.php");
	}	
	
	/* Hold on to ourselves, just in case the list is refreshed */
	[self retain];

	/* Remove incident from list */
	if ([self.metric.customer.incidentList.incidents containsObject:self])
	{
		[self.metric.customer.incidentList.incidents removeObject:self];
		[self.metric.customer.incidentList.incidentDict removeObjectForKey:[NSString stringWithFormat:@"%i", self.identifier]];
	}
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	[super connection:connection didFailWithError:error];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:@"IncidentClearingFinished" object:self];
	
	[self release];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
	NSLog (@"Received %@", [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding]);
	
	/* Clean-up */
    [receivedData release];
	receivedData = nil;
	
	/* Post Notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"IncidentClearingFinished" object:self];
	
	/* Refresh Incident List */
	NSLog (@"Done clearing, refreshing incident list %@", self.metric.customer.incidentList);
	[self.metric.customer.incidentList refresh];
	
	/* Release self */
	[self release];
}


@synthesize identifier;
@synthesize startDate;
@synthesize endDate;
@synthesize caseIdentifier;
@synthesize raisedValue;
@synthesize resourceAddress;
@synthesize entityDescriptor;
@synthesize metric;
@synthesize actions;
@synthesize actionDict;

@end
