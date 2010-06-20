//
//  LCGroupTree.m
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGroupTree.h"
#import "LCCustomer.h"
#import "LCGroup.h"

@interface LCGroupTree (private)
- (void) recursivelyCheckForObsolescenceInGroup:(LCGroup *)group olderThan:(NSDate *)refreshVersion;
- (void) recursivelyRemoveRecentChildrenInGroup:(LCGroup *)group;
@end

@implementation LCGroupTree

#pragma mark "Constructors"

- (id) initWithCustomer:(id)initCustomer
{
	self = [super init];
	if (!self) return nil;
	
	self.customer = initCustomer;
	groups = [[NSMutableArray array] retain];
	groupDictionary = [[NSMutableDictionary dictionary] retain];
	
	return self;
}

- (void) dealloc
{
	[groups release];
	[groupDictionary release];
	[super dealloc];
}

#pragma mark "Refresh"

- (void) refreshWithPriority:(int)priority
{
	/* Check state */
	if (!customer) return;
	if (refreshXMLRequest) return;
	
	/* Create XML Request */
	refreshXMLRequest = [[LCXMLRequest requestWithCriteria:customer
												  resource:[(LCCustomer *)customer resourceAddress] 
													entity:[(LCCustomer *)customer entityAddress] 
												   xmlname:@"group_list" 
													refsec:0 
													xmlout:nil] retain];
	[refreshXMLRequest setDelegate:self];
	[refreshXMLRequest setThreadedXmlDelegate:self];
	[refreshXMLRequest setPriority:priority];
	[refreshXMLRequest performAsyncRequest];
	self.refreshInProgress = YES;
}

- (void) highPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_HIGH]; }

- (void) normalPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_NORMAL]; }

- (void) lowPriorityRefresh
{ [self refreshWithPriority:XMLREQ_PRIO_LOW]; }

- (void) xmlParserDidFinish:(LCXMLNode *)rootNode
{
	NSMutableArray *newGroups = [NSMutableArray array];
	NSDate *refreshVersion = [NSDate date];
	
	/* Clear out the recentChildren arrays */
	for (LCGroup *group in groups)
	{
		[self recursivelyRemoveRecentChildrenInGroup:group];
	}
	
	/* Parse received XML */
	for (LCXMLNode *childNode in rootNode.children)
	{ 
		if ([childNode.name isEqualToString:@"group"])
		{
			LCGroup *group = [groupDictionary objectForKey:[childNode.properties objectForKey:@"id"]];
			if (!group)
			{
				/* Create new */
				group = [LCGroup new];
				group.customer = self.customer;
				group.groupID = [[childNode.properties objectForKey:@"id"] intValue];
				group.parentID = [[childNode.properties objectForKey:@"parent"] intValue];
				[newGroups addObject:group];
				[groupDictionary setObject:group forKey:[childNode.properties objectForKey:@"id"]];
				[group autorelease];
			}
			[group setXmlValuesUsingXmlNode:childNode];
			group.refreshVersion = refreshVersion;
		}
		else if ([childNode.name isEqualToString:@"entity"])
		{
			LCEntityDescriptor *entityDesc = nil;
			for (LCXMLNode *entNode in childNode.children)
			{
				if ([entNode.name isEqualToString:@"entity_descriptor"])
				{ 
					entityDesc = [LCEntityDescriptor descriptorWithXmlNode:entNode]; 
					break;
				}
			}			
			LCGroup *parent = [groupDictionary objectForKey:[childNode.properties objectForKey:@"parent"]];
			LCEntity *entity = [entityDesc locateEntity:YES];
			
			if (entity.type > 3) 
			{
				[entity normalPriorityRefresh];
			}
			if (![parent.childrenDictionary objectForKey:[entityDesc addressString]] && entity)
			{
				/* New Entity */
				[parent insertObject:entity inChildrenAtIndex:parent.children.count];
				[parent.childrenDictionary setObject:entity forKey:[entityDesc addressString]];
			}
			if (entity)
			{ 
				[parent.recentChildren addObject:entity]; 
			}
		}
	}
	
	/* Position new groups in the hierarchy */
	for (LCGroup *newGroup in newGroups)
	{
		if (newGroup.parentID > 0)
		{
			/* Non-Root Level */
			newGroup.parent = [groupDictionary objectForKey:[NSString stringWithFormat:@"%i", newGroup.parentID]];
			[newGroup.parent insertObject:newGroup inChildrenAtIndex:newGroup.parent.children.count];
			[newGroup.parent.childrenDictionary setObject:newGroup forKey:[NSString stringWithFormat:@"%i", newGroup.parentID]];
		}
		else
		{
			/* Root-level */
			[self insertObject:newGroup inGroupsAtIndex:groups.count];
			[groupDictionary setObject:newGroup forKey:[NSString stringWithFormat:@"%i", newGroup.parentID]];
		}
	}	
	
	/* Check for obsolete groups and entities */
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCGroup *childGroup in self.groups)
	{
		/* Check for obsolete root-level groups */
		if (!childGroup.refreshVersion || [childGroup.refreshVersion compare:refreshVersion] == NSOrderedAscending)
		{
			[removeArray addObject:childGroup];
		}
		
		/* Check children */
		[self recursivelyCheckForObsolescenceInGroup:childGroup olderThan:refreshVersion];
	}
	for (LCGroup *childGroup in removeArray)
	{
		[self.groupDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", childGroup.parentID]];
		[self removeObjectFromGroupsAtIndex:[self.groups indexOfObject:childGroup]];		
	}
}

- (void) recursivelyCheckForObsolescenceInGroup:(LCGroup *)group olderThan:(NSDate *)refreshVersion
{
	/* Check each child */
	NSMutableArray *groupRemoveArray = [NSMutableArray array];
	NSMutableArray *entityRemoveArray = [NSMutableArray array];
	for (id child in group.children)
	{
		if ([[child class] isSubclassOfClass:[LCEntity class]])
		{
			LCEntity *entity = (LCEntity *) child;
			if (![group.recentChildren containsObject:entity])
			{
				/* Entity is obsolete */
				[entityRemoveArray addObject:entity];
			}
		}
		else if ([[child class] isSubclassOfClass:[LCGroup class]])
		{
			LCGroup *childGroup = (LCGroup *) child;
			if (!childGroup.refreshVersion || [childGroup.refreshVersion compare:refreshVersion] == NSOrderedAscending)
			{
				/* Group is obsolete */
				[groupRemoveArray addObject:childGroup];
			}
			else
			{
				[self recursivelyCheckForObsolescenceInGroup:childGroup olderThan:refreshVersion];
			}
		}
	}
	for (LCGroup *childGroup in groupRemoveArray)
	{
		[group.childrenDictionary removeObjectForKey:[NSString stringWithFormat:@"%i", childGroup.parentID]];
		[group removeObjectFromChildrenAtIndex:[group.children indexOfObject:childGroup]];		
	}
	for (LCEntity *entity in entityRemoveArray)
	{
		[group.childrenDictionary removeObjectForKey:[[entity entityDescriptor] addressString]];
		[group removeObjectFromChildrenAtIndex:[group.children indexOfObject:entity]];		
	}
}

- (void) recursivelyRemoveRecentChildrenInGroup:(LCGroup *)group
{
	[group.recentChildren removeAllObjects];
	for (id child in group.children)
	{
		if ([[child class] isSubclassOfClass:[LCGroup class]])
		{ [self recursivelyRemoveRecentChildrenInGroup:child]; }
	}
}


- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	/* Free XML request */
	[sender release];
	refreshXMLRequest = nil;
	
	/* Set refresh flag */
	self.refreshInProgress = NO;
}


#pragma mark "Properties"

@synthesize customer;

@synthesize groups;
- (void) insertObject:(id)group inGroupsAtIndex:(unsigned int)index
{
	[groups insertObject:group atIndex:index];
}
- (void) removeObjectFromGroupsAtIndex:(unsigned int)index;
{
	[groups removeObjectAtIndex:index];
}
@synthesize groupDictionary;

@synthesize refreshInProgress;


@end
