//
//  LCBrowserTreeCoreCustomer.m
//  Lithium Console
//
//  Created by James Wilson on 3/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserTreeCoreCustomer.h"

#import "LCBrowserTreeCoreProperty.h"
#import "LCBrowserSiteContentController.h"
#import "LCBrowserUsersContentController.h"
#import "LCBrowserLicenseContentController.h"
#import "LCBrowserServiceScriptsContentController.h"
#import "LCBrowserActionScriptsContentController.h"
#import "LCBrowserModBScriptsContentController.h"
#import "LCBrowserActionsContentController.h"

@interface LCBrowserTreeCoreCustomer (private)

- (void) updateTreeIcon;

@end

@implementation LCBrowserTreeCoreCustomer

#pragma mark "Constructors"

- (LCBrowserTreeCoreCustomer *) initWithCustomer:(LCCustomer *)initCustomer
{
	[super init];
	
	children = [[NSMutableArray array] retain];
	self.customer = initCustomer;
	self.displayString = customer.desc;
	self.uniqueIdentifier = customer.name;
	[self updateTreeIcon];
	
	self.isBrowserTreeLeaf = NO;
	
	/*
	 * Create properties 
	 */
	
	LCBrowserTreeCoreProperty *property;
	
	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"License Key";
	property.contentControllerClass = [LCBrowserLicenseContentController class];
	property.treeIcon = [NSImage imageNamed:@"mortgage_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];	
	
	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"Users";
	property.contentControllerClass = [LCBrowserUsersContentController class];
	property.treeIcon = [NSImage imageNamed:@"user_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];

	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"Actions";
	property.contentControllerClass = [LCBrowserActionsContentController class];
	property.treeIcon = [NSImage imageNamed:@"tools_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];
	
	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"Action Scripts";
	property.contentControllerClass = [LCBrowserActionScriptsContentController class];
	property.treeIcon = [NSImage imageNamed:@"tools_script_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];	

	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"Service Scripts";
	property.contentControllerClass = [LCBrowserServiceScriptsContentController class];
	property.treeIcon = [NSImage imageNamed:@"go_script_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];	

	property = [(LCBrowserTreeCoreProperty *)[LCBrowserTreeCoreProperty alloc] initWithCustomer:customer];
	property.displayString = @"Custom Modules";
	property.contentControllerClass = [LCBrowserModBScriptsContentController class];
	property.treeIcon = [NSImage imageNamed:@"modb_16.tif"];
	[self insertObject:property inChildrenAtIndex:children.count];
	[property autorelease];		
	
	/* Observe change in deployment reachability and usabiliy */
	[customer addObserver:self 
			   forKeyPath:@"refreshSuccessful"
				  options:NSKeyValueObservingOptionNew
				  context:NULL];
	[customer addObserver:self 
			   forKeyPath:@"disabled"
				  options:NSKeyValueObservingOptionNew
				  context:NULL];	
	
	return self;
}

- (void) dealloc
{
	[customer removeObserver:self forKeyPath:@"refreshSuccessful"];
	[customer removeObserver:self forKeyPath:@"disabled"];
	[customer release];
	[super dealloc];
}

#pragma mark "Status Methods"

- (void) updateTreeIcon
{
	if (customer.refreshSuccessful && !customer.disabled)
	{ 
		if (self.isBrowserTreeLeaf) self.isBrowserTreeLeaf = NO; 
		if (self.opState != -2) self.opState = -2;
	}
	else
	{ 
		if (!self.isBrowserTreeLeaf) self.isBrowserTreeLeaf = YES;
		if (self.opState != -1) self.opState = -1;
	}
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"refreshSuccessful"] || [keyPath isEqualToString:@"disabled"])
	{
		[self updateTreeIcon];
	}	
}

#pragma mark "Properties"

@synthesize customer;


@end
