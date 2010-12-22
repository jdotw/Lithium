//
//  LTCustomer.m
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTCustomer.h"
#import "AppDelegate.h"
#import "LTAuthenticationTableViewController.h"
#import "LTPushRegistrationRequest.h"
#import "LTGroupTree.h"

@implementation LTCustomer

@synthesize customModules;
@synthesize url;
@synthesize cluster;
@synthesize node;
@synthesize coreVersionMajor, coreVersionMinor, coreVersionPoint;

- (LTCustomer *) init
{
	[super init];
	
	self.type = 1;
	self.incidentList = [LTIncidentList new];
	self.incidentList.customer = self;
	self.customer = self;
	self.groupTree = [LTGroupTree new];
	[groupTree setGroupID:0];
	[groupTree setParent:self];
	[groupTree setCustomer:self];
	
	return self;
}

- (void) dealloc
{
	[url release];
	[cluster release];
	[node release];
	[customModules release];
	[super dealloc];
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	[super xmlParserDidFinish:rootNode];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTCustomerRefreshFinished" object:self];

	/* Parse Custom Module List */
	self.customModules = [NSMutableDictionary dictionary];
	for (LCXMLNode *entityNode in rootNode.children)
	{
		
		if ([entityNode.name isEqualToString:@"entity"])
		{
			if ([entityNode.properties objectForKey:@"core_version"])
			{
				NSArray *components = [[entityNode.properties objectForKey:@"core_version"] componentsSeparatedByString:@"."];
				if (components.count > 0) coreVersionMajor = [[components objectAtIndex:0] intValue];
				if (components.count > 1) coreVersionMinor = [[components objectAtIndex:1] intValue];
				if (components.count > 2) coreVersionPoint = [[components objectAtIndex:2] intValue];
			}
			for (LCXMLNode *listNode in entityNode.children)
			{
				if ([listNode.name isEqualToString:@"custom_module_list"])
				{
					for (LCXMLNode *vendorNode in listNode.children)
					{
						[self.customModules setObject:[vendorNode.properties objectForKey:@"desc"] 
											   forKey:[vendorNode.properties objectForKey:@"name"]];
					}
				}
			}
		}
	}
}

- (NSString *) resourceAddress
{ return [NSString stringWithFormat:@"%@:%@:5:0:%@", cluster, node, self.name]; }

- (NSString *) entityAddress
{ return [NSString stringWithFormat:@"1:%@", self.name]; }

@synthesize incidentList;

- (void) setUuidString:(NSString *)value
{
	if ([value isEqualToString:self.uuidString]) return;	
	[super setUuidString:value];
	
	if ([(AppDelegate *)[[UIApplication sharedApplication] delegate] pushToken])
	{
		LTPushRegistrationRequest *pushReq = [[LTPushRegistrationRequest alloc] initWithCustomer:self
																						   token:[(AppDelegate *)[[UIApplication sharedApplication] delegate] pushToken]
																			receiveNotifications:YES];
		[pushReq performRequest];
	}
	
}											  

@synthesize groupTree;


@end
