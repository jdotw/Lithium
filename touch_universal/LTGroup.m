//
//  LTGroup.m
//  Lithium
//
//  Created by James Wilson on 18/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTGroup.h"
#import "AppDelegate.h"
#import "LTAuthenticationTableViewController.h"

@implementation LTGroup

#pragma mark "Constructors"

- (LTGroup *) init
{
	[super init];

	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Refresh"

- (void) refresh
{
	/* Refresh the group */
	
	/* Check state */
	if (refreshInProgress || ![(LTCoreDeployment *)customer.coreDeployment enabled])
	{
		return;
	}
	if (lastRefresh && [[NSDate date] timeIntervalSinceDate:lastRefresh] < 60.0)
	{
		/* Enforce a maximum 1-minute refresh interval */
		return;
	}
	
	/* Create outbound XML */
	NSMutableString *xmlString;
	xmlString = [NSMutableString stringWithFormat:@"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"];
	[xmlString appendString:@"<group_list>"];
	[xmlString appendFormat:@"<id>%i</id>", self.groupID];
	[xmlString appendString:@"</group_list>"];	
	
	/* Create request */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[customer urlForXml:@"group_list" timestamp:0]
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
	[postData appendData:[@"Content-Type: text/plain\r\n\r\n" dataUsingEncoding:NSUTF8StringEncoding]];
	[xmlString replaceOccurrencesOfString:@"\r\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[xmlString replaceOccurrencesOfString:@"\n" withString:@"&#xD;&#xA;" options:NSCaseInsensitiveSearch range:NSMakeRange(0,[xmlString length])];
	[postData appendData:[xmlString dataUsingEncoding:NSUTF8StringEncoding]];
	[postData appendData:[[NSString stringWithFormat:@"\r\n--%@--\r\n", formBoundary] dataUsingEncoding:NSUTF8StringEncoding]];
	[theRequest setHTTPBody:postData];
	
	/* Begin download */
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) 
	{
		self.refreshInProgress = YES;
		receivedData=[[NSMutableData data] retain];
	} 
	else 
	{
		/* FIX */
		NSLog (@"ERROR: Failed to download group list");
	}	
}

- (void) updateEntityUsingXML:(TBXML *)xml
{
	/* Loop through XML nodes */
	NSMutableArray *seenGroups = [NSMutableArray array];
	NSMutableArray *seenEntities = [NSMutableArray array];
    TBXMLElement *node = xml.rootXMLElement;
    for (node=node->firstChild; node; node = node->nextSibling)
    {
        NSString *nodeName = [TBXML elementName:node];
		if ([nodeName isEqualToString:@"group"]) 
        {
			/* Child Group */
			LTGroup *childGroup = [childDict objectForKey:[TBXML textForElementNamed:@"id" parentElement:node]];
			if (!childGroup)
			{
				/* Create new group */
				childGroup = [LTGroup new];
                childGroup.groupID = [TBXML intFromTextForElementNamed:@"id" parentElement:node];
				childGroup.parent = self;
				childGroup.customer = self.customer;
				[children addObject:childGroup];
				[childDict setObject:childGroup forKey:[NSString stringWithFormat:@"%i", childGroup.groupID]];
			}
			childGroup.desc = [TBXML textForElementNamed:@"desc" parentElement:node];
			[seenGroups addObject:childGroup];
		}
		else if ([nodeName isEqualToString:@"entity"])
        {
			/* Entity (contains <parent> and <entity descriptor> */
            TBXMLElement *entDescNode = [TBXML childElementNamed:@"entity_descriptor" parentElement:node];
            if (entDescNode)
            {
				/* Interpret entityDescriptor */
				LTEntityDescriptor *childEntDesc = [LTEntityDescriptor entityDescriptorFromXml:entDescNode]; 
				
				/* Check for existing entity */
				LTEntity *entity = [childDict objectForKey:childEntDesc.entityAddress];
				if (!entity)
				{					
					/* Create stand-alone entity for the group */
					entity = [LTEntity new];
					entity.type = childEntDesc.type;
					entity.name = childEntDesc.name;
					entity.username = [customer username];
					entity.password = [customer password];
					entity.customer = customer;
					entity.customerName = customer.name;
					entity.resourceAddress = customer.resourceAddress;
					entity.ipAddress = customer.ipAddress;
					entity.coreDeployment = customer.coreDeployment;
					entity.entityDescriptor = childEntDesc;
					entity.entityAddress = childEntDesc.entityAddress;
					[children addObject:entity];
					[childDict setObject:entity forKey:[entity.entityDescriptor entityAddress]];
				}
				entity.desc = childEntDesc.desc;
				entity.opState = childEntDesc.opState;
				entity.adminState = childEntDesc.adminState;
				[seenEntities addObject:entity];
			}
		}
	}	
	
	/* Check for obsolete objects */
	NSMutableArray *removeGroups = [NSMutableArray array];
	NSMutableArray *removeEntities = [NSMutableArray array];
	for (id item in children)
	{
		if ([item class] == [LTGroup class])
		{
			LTGroup *group = (LTGroup *)item;
			if (![seenGroups containsObject:group])
			{ [removeGroups addObject:group]; }
		}
		else if ([item class] == [LTEntity class])
		{
			LTEntity *entity = (LTEntity *)item;
			if (![seenEntities containsObject:entity])
			{ [removeEntities addObject:entity]; }
		}
	}
	for (LTGroup *group in removeGroups)
	{ [children removeObjectAtIndex:[children indexOfObject:group]]; }
	for (LTEntity *entity in removeEntities)
	{ [children removeObjectAtIndex:[children indexOfObject:entity]]; }	
}

@synthesize groupID;
@synthesize parentID;

@end
