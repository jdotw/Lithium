//
//  LCUserList.m
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCUserList.h"
#import "LCCustomer.h"
#import "LCUser.h"

@implementation LCUserList

#pragma mark Constructors

+ (LCUserList *) userListWithCustomer:(id)initCustomer
{
	return [[[LCUserList alloc] initWithCustomer:initCustomer] autorelease];
}

- (id) initWithCustomer:(id)initCustomer
{
	self = [self init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	
	return self;
}

- (id) init
{
	[super init];
	
	users = [[NSMutableArray array] retain];
	userDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[users release];
	[userDictionary release];
	if (refreshXMLRequest) { [refreshXMLRequest cancel]; [refreshXMLRequest release]; }
	if (curXMLDictionary) [curXMLDictionary release];
	if (curXMLString) [curXMLString release];
	[super dealloc];
}

#pragma mark "Refresh methods"

- (void) refreshWithPriority:(int)priority
{
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[(LCCustomer *)customer resourceAddress] 
													entity:[(LCCustomer *)customer entityAddress] 
												   xmlname:@"user_list"
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	
	[self setRefreshInProgress:YES];
}

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	NSMutableArray *usersSeen = [NSMutableArray array];
	for (LCXMLNode *childNode in rootNode.children)
	{ 
		if ([childNode.name isEqualToString:@"user"])
		{
			LCUser *user = [userDictionary objectForKey:[childNode.properties objectForKey:@"username"]];
			if (!user)
			{
				user = [LCUser new];
				user.username = [childNode.properties objectForKey:@"username"];
				user.customer = customer;
				[self insertObject:user inUsersAtIndex:users.count];
				[user autorelease];
			}
			[user setXmlValuesUsingXmlNode:childNode];
			[usersSeen addObject:user];
		}
	}
	
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCUser *user in users)
	{
		if (![usersSeen containsObject:user])
		{
			/* User is obsolete */
			[removeArray addObject:user];
		}
	}
	for (LCUser *user in removeArray)
	{
		[self removeObjectFromUsersAtIndex:[users indexOfObject:user]];
	}
}

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{	
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	[self setRefreshInProgress:NO];
}

#pragma mark "User Methods"

@synthesize users;
- (void) insertObject:(id)user inUsersAtIndex:(unsigned int)index
{ 
	[userDictionary setObject:user forKey:[user username]];
	[users insertObject:user atIndex:index];
}

- (void) removeObjectFromUsersAtIndex:(unsigned int) index
{ 
	if ([users objectAtIndex:index])
	{ [userDictionary removeObjectForKey:[[users objectAtIndex:index] username]]; }
	[users removeObjectAtIndex:index];
}
@synthesize userDictionary;

#pragma mark "General Properties"

@synthesize customer;
@synthesize refreshInProgress;

@end
