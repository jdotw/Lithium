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
#import "TBXML-Lithium.h"

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

- (void) updateEntityUsingXMLNode:(TBXMLElement *)rootNode
{
    [super updateEntityUsingXMLNode:rootNode];

    /* Post customer-specific notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTCustomerRefreshFinished" object:self];

	/* Parse Custom Module List */
	self.customModules = [NSMutableDictionary dictionary];
    if ([TBXML childElementNamed:@"core_version" parentElement:rootNode])
    {
        NSArray *components = [[TBXML textForElementNamed:@"core_version" parentElement:rootNode] componentsSeparatedByString:@"."];
        if (components.count > 0) coreVersionMajor = [[components objectAtIndex:0] intValue];
        if (components.count > 1) coreVersionMinor = [[components objectAtIndex:1] intValue];
        if (components.count > 2) coreVersionPoint = [[components objectAtIndex:2] intValue];
    }
    TBXMLElement *moduleList = [TBXML childElementNamed:@"custom_module_list" parentElement:rootNode];
    if (moduleList)
    {
        TBXMLElement *moduleNode;
        for (moduleNode = [TBXML childElementNamed:@"module" parentElement:moduleList]; 
             moduleNode; 
             moduleNode = [TBXML nextSiblingNamed:@"module" searchFromElement:moduleNode])
        {
            [self.customModules setObject:[TBXML textForElementNamed:@"desc" parentElement:moduleNode]
                                   forKey:[TBXML textForElementNamed:@"name" parentElement:moduleNode]];
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
