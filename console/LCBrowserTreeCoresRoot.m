//
//  LCBrowserTreeCoresRoot.m
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserTreeCoresRoot.h"
#import "LCBrowserTreeCoreDeployment.h"
#import "LCCustomerList.h"
#import "LCCoreDeployment.h"
#import "LCBrowserTreeCoreDeployment.h"

@implementation LCBrowserTreeCoresRoot

#pragma mark "Constructors"

- (LCBrowserTreeCoresRoot *) init
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	for (LCCoreDeployment *core in [[LCCustomerList masterList] staticDeployments])
	{
		LCBrowserTreeCoreDeployment *coreItem = [[LCBrowserTreeCoreDeployment alloc] initWithDeployment:core];
		[self insertObject:coreItem inChildrenAtIndex:children.count];
		[coreItem autorelease];
	}
	for (LCCoreDeployment *core in [[LCCustomerList masterList] dynamicDeployments])
	{
		LCBrowserTreeCoreDeployment *coreItem = [[LCBrowserTreeCoreDeployment alloc] initWithDeployment:core];
		[self insertObject:coreItem inChildrenAtIndex:children.count];
		[coreItem autorelease];		
	}
	
	/* Listen for changes in the deployment lists */
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"staticDeployments" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];	
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"dynamicDeployments" 
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];	
	
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"staticDeployments"];
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"dynamicDeployments"];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the container list */
	int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
	if (changeType == NSKeyValueChangeInsertion)
	{
		for (LCCoreDeployment *core in [change objectForKey:NSKeyValueChangeNewKey])
		{ 
			LCBrowserTreeCoreDeployment *coreItem = [[LCBrowserTreeCoreDeployment alloc] initWithDeployment:core];
			[self insertObject:coreItem inChildrenAtIndex:children.count];
			[coreItem autorelease];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:@"BrowserTreeItemAdded" object:self];
	}
	else if (changeType == NSKeyValueChangeRemoval)
	{
		for (LCCoreDeployment *core in [change objectForKey:NSKeyValueChangeOldKey])
		{ 
			NSMutableArray *removeDeployments = [NSMutableArray array];
			for (LCBrowserTreeCoreDeployment *deploymentItem in children)
			{ 
				if (deploymentItem.deployment == core)
				{ [removeDeployments addObject:deploymentItem]; }
			}
			for (LCBrowserTreeCoreDeployment *deploymentItem in removeDeployments)
			{
				[self removeObjectFromChildrenAtIndex:[children indexOfObject:deploymentItem]];	
			}
		}
	}
}


#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"LITHIUM DEPLOYMENTS";
}

@end
