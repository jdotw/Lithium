//
//  LCBrowserTreeCoreDeployment.m
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserTreeCoreDeployment.h"
#import "LCBrowserTreeCoreCustomer.h"

@interface LCBrowserTreeCoreDeployment (private)

- (void) updateTreeIcon;

@end

@implementation LCBrowserTreeCoreDeployment

#pragma mark "Constructors"

- (LCBrowserTreeCoreDeployment *) initWithDeployment:(LCCoreDeployment *)initDeployment
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	self.deployment = initDeployment;
	self.displayString = deployment.address;
	self.uniqueIdentifier = deployment.address;

	[self updateTreeIcon];
	
	for (LCCustomer *customer in deployment.customers)
	{ 
		LCBrowserTreeCoreCustomer *customerItem = [(LCBrowserTreeCoreCustomer *)[LCBrowserTreeCoreCustomer alloc] initWithCustomer:customer];
		[self insertObject:customerItem inChildrenAtIndex:children.count];
		[customerItem autorelease];
	}
	
	/* Observe change in customers */
	[deployment addObserver:self 
				 forKeyPath:@"customers"
					options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					context:NULL];
	
	/* Observe change in deployment reachability and usabiliy */
	[deployment addObserver:self 
				 forKeyPath:@"reachable"
					options:NSKeyValueObservingOptionNew
					context:NULL];
	[deployment addObserver:self 
				 forKeyPath:@"disabled"
					options:NSKeyValueObservingOptionNew
					context:NULL];
	
	
	if (children.count > 0) self.isBrowserTreeLeaf = NO;
	else self.isBrowserTreeLeaf = YES;
	
	return self;
}

- (void) dealloc
{
	[deployment removeObserver:self forKeyPath:@"customers"];
	[deployment removeObserver:self forKeyPath:@"reachable"];
	[deployment removeObserver:self forKeyPath:@"disabled"];
	[deployment release];
	[super dealloc];
}

#pragma mark "Status Methods"

- (void) updateTreeIcon
{
//	if (deployment.reachable && !deployment.disabled)
//	{ 
//		self.treeIcon = [NSImage imageNamed:@"BlueDot.tiff"]; 
//		self.opState = -2;
//	}
//	else
//	{ 
//		self.treeIcon = [NSImage imageNamed:@"GreyDot.tiff"]; 
//		self.opState = -1;
//	}
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the customer list */
	if ([keyPath isEqualToString:@"customers"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeNewKey])
			{ 
				LCBrowserTreeCoreCustomer *customerItem = [(LCBrowserTreeCoreCustomer *)[LCBrowserTreeCoreCustomer alloc] initWithCustomer:customer];
				[self insertObject:customerItem inChildrenAtIndex:children.count];
				[customerItem autorelease];
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeOldKey])
			{ 
				NSMutableArray *removeArray = [NSMutableArray array];
				for (LCBrowserTreeCoreCustomer *customerItem in children)
				{
					if (customerItem.customer == customer)
					{ [removeArray addObject:customerItem]; }
				}
				for (LCBrowserTreeCoreCustomer *customerItem in removeArray)
				{ [self removeObjectFromChildrenAtIndex:[children indexOfObject:customerItem]]; }
			}
		}

		if (children.count > 0) self.isBrowserTreeLeaf = NO;
		else self.isBrowserTreeLeaf = YES;
	}
	else if ([keyPath isEqualToString:@"reachable"] || [keyPath isEqualToString:@"disabled"])
	{
		[self updateTreeIcon];
	}	
}

#pragma mark "Properties"

@synthesize deployment;

@end