//
//  LTGroupTree.m
//  Lithium
//
//  Created by James Wilson on 19/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LTGroupTree.h"

#import "LTGroup.h"

@interface LTGroup (private)
- (void) recursivelyCheckObsoleteGroups:(NSArray *)seenGroups 
							andEntities:(NSArray *)seenEntities
								inGroup:(LTGroup *)group;
@end

@implementation LTGroupTree

#pragma mark "Constructors"

- (LTGroupTree *) init
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
	
	/* Create request */
	NSMutableURLRequest *theRequest= [NSMutableURLRequest requestWithURL:[customer urlForXml:@"group_list" timestamp:0]
															 cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
														 timeoutInterval:60.0];
	
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
	/* Interpret */
	NSMutableArray *newGroups = [NSMutableArray array];
	NSMutableArray *newEntities = [NSMutableArray array];
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
				childGroup.parentID = [TBXML intFromTextForElementNamed:@"parent" parentElement:node];
				childGroup.customer = customer;
				[childDict setObject:childGroup forKey:[NSString stringWithFormat:@"%i", childGroup.groupID]];
				[newGroups addObject:childGroup];
			}
			childGroup.desc = [TBXML textForElementNamed:@"desc" parentElement:node];
			[seenGroups addObject:childGroup];
		}
		else if ([nodeName isEqualToString:@"entity"])
		{
			/* Entity */
            TBXMLElement *entDescNode = [TBXML childElementNamed:@"entity_descriptor" parentElement:node];
            if (entDescNode)
            {
				/* Interpret entityDescriptor */
				LTEntityDescriptor *childEntDesc = [LTEntityDescriptor entityDescriptorFromXml:entDescNode]; 
				
				/* Get Parent Group */
				LTGroup *parentGroup = [childDict objectForKey:[TBXML textForElementNamed:@"parent" parentElement:node]];
				
				/* Check for existing entity */
				LTEntity *entity = [parentGroup.childDict objectForKey:childEntDesc.entityAddress];
				if (!entity)
				{					
					/* Create stand-alone entity for the group */
					entity = [LTEntity new];
					entity.parent = parentGroup;
					entity.type = childEntDesc.type;
					entity.name = childEntDesc.name;
					entity.username = [customer username];
					entity.password = [customer password];
					entity.customer = customer;
					entity.customerName = customer.name;
					entity.resourceAddress = [TBXML textForElementNamed:@"resaddr" parentElement:entDescNode] ? : customer.resourceAddress;
					entity.ipAddress = customer.ipAddress;
					entity.coreDeployment = customer.coreDeployment;
					entity.entityDescriptor = childEntDesc;
					entity.entityAddress = childEntDesc.entityAddress;
                    NSLog (@"entityAddress is %@", entity.entityAddress);
					[parentGroup.children addObject:entity];
					[parentGroup.childDict setObject:entity forKey:entity.entityAddress];
					[newEntities addObject:entity];
				}
				entity.desc = childEntDesc.desc;
				entity.opState = childEntDesc.opState;
				entity.adminState = childEntDesc.adminState;
				[seenEntities addObject:entity];
			}
		}
	}	
	
	/* Position new groups in hierarchy */
	for (LTGroup *group in newGroups)
	{
		if (group.parentID > 0)
		{
			LTGroup *parentGroup = [childDict objectForKey:[NSString stringWithFormat:@"%i", group.parentID]];
			[parentGroup.children addObject:group];
			[parentGroup.childDict setObject:group forKey:[NSString stringWithFormat:@"%i", group.groupID]];
			group.parent = parentGroup;
		}
		else
		{
			[children addObject:group];
		}
	}
	
	/* Set indent level for new groups */
	for (LTGroup *group in newGroups)
	{
		int indent = 0;
		LTGroup *parentGroup = (LTGroup *) group.parent;
		while (parentGroup)
		{
			indent++;
			parentGroup = (LTGroup *) parentGroup.parent;
		}
		group.indentLevel = indent;
		
	}	
	
	/* Set indent level for new entities */
	for (LTEntity *entity in newEntities)
	{
		entity.indentLevel = entity.parent.indentLevel + 1;
	}
	
	/* Check for obsolete objects */
	[self recursivelyCheckObsoleteGroups:seenGroups
							 andEntities:seenEntities 
								 inGroup:self]; 

	/* Post group-tree specific notification */
	[[NSNotificationCenter defaultCenter] postNotificationName:@"LTGroupTreeRefreshFinished" object:self];
}

- (void) recursivelyCheckObsoleteGroups:(NSArray *)seenGroups 
							andEntities:(NSArray *)seenEntities
								inGroup:(LTGroup *)group
{
	NSMutableArray *removeGroups = [NSMutableArray array];
	NSMutableArray *removeEntities = [NSMutableArray array];
	for (id item in group.children)
	{
		if ([item class] == [LTGroup class])
		{
			/* Group */
			LTGroup *childGroup = (LTGroup *)item;
			if (![seenGroups containsObject:childGroup])
			{ 
				[removeGroups addObject:childGroup];
			}
			else 
			{ 
				[self recursivelyCheckObsoleteGroups:seenGroups
										 andEntities:seenEntities 
											 inGroup:childGroup]; 
			}
		}
		if ([item class] == [LTEntity class])
		{
			/* Entity */
			LTEntity *childEntity = (LTEntity *)item;
			if (![seenEntities containsObject:childEntity])
			{
				[removeEntities addObject:childEntity];
			}
		}
	}
	for (LTEntity *childEntity in removeEntities)
	{
		[group.children removeObjectAtIndex:[group.children indexOfObject:childEntity]];
		[group.childDict removeObjectForKey:childEntity.entityAddress];		
	}
	for (LTGroup *childGroup in removeGroups)
	{
		[group.children removeObjectAtIndex:[group.children indexOfObject:childGroup]]; 
		[group.childDict removeObjectForKey:[NSString stringWithFormat:@"%i", childGroup.groupID]];	
	}
}

#pragma mark "Properties"

- (void) setCustomer:(LTCustomer *)value
{
	[super setCustomer:value];
	
	self.name = customer.name;
	self.desc = customer.desc;
}

@end

