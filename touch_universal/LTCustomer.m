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
	[super dealloc];
}

@synthesize url;
@synthesize cluster;
@synthesize node;

- (NSString *) resourceAddress
{ return [NSString stringWithFormat:@"%@:%@:5:0:%@", cluster, node, name]; }

- (NSString *) entityAddress
{ return [NSString stringWithFormat:@"1:%@", name]; }

@synthesize incidentList;

- (void) setUuidString:(NSString *)value
{
	if ([value isEqualToString:uuidString]) return;
	
	[uuidString release];
	uuidString = [value copy];

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
