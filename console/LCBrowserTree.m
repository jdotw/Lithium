//
//  LCBrowserTree.m
//  Lithium Console
//
//  Created by James Wilson on 30/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTree.h"

#import "LCCustomerList.h"
#import "LCBrowserTreeDevicesRoot.h"
#import "LCBrowserTreeXsanRoot.h"
#import "LCBrowserTreeFaultsRoot.h"
#import "LCBrowserTreeDiscoveryRoot.h"
#import "LCBrowserTreeDocumentsRoot.h"
#import "LCBrowserTreeApplicationsRoot.h"
#import "LCBrowserTreeGroupsRoot.h"
#import "LCBrowserTreeCoresRoot.h"

@interface LCBrowserTree (private)


@end


@implementation LCBrowserTree

#pragma mark "Constructor"

+ (LCBrowserTree *) browserTree
{
	return [[[LCBrowserTree alloc] init] autorelease];
}

- (LCBrowserTree *) init
{ 
	[super init];
	
	/* Create items */
	self.items = [NSMutableArray array];

	/* Observe Customer List */
	[[LCCustomerList masterList] addObserver:self forKeyPath:@"array" options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) context:nil];

	/* Observe Tree Item Additions */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(treeItemAdded:)
												 name:@"BrowserTreeItemAdded"
											   object:nil];	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(rootItemEnabledChanged:)
												 name:@"LCBrowserTreeItemRootEnabledChanged"
											   object:nil];	
	
	/* Default Sort Descriptor */
	self.sortDescriptors = [NSArray arrayWithObject:[[[NSSortDescriptor alloc] initWithKey:@"sortString" ascending:YES] autorelease]];
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"BrowserTreeItemAdded" object:nil];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"LCBrowserTreeItemRootEnabledChanged" object:nil];
	[items release];
	[super dealloc];
}

#pragma mark "Root Item Management"

- (void) resetRootItems
{
	NSMutableArray *newItems = [NSMutableArray array];

	/* Devices */
	self.devicesRoot = [[LCBrowserTreeDevicesRoot new] autorelease];
	[newItems addObject:devicesRoot];

	/* Groups */
	self.groupsRoot = [[LCBrowserTreeGroupsRoot new] autorelease];
	[newItems addObject:groupsRoot];
	
	/* Applications */
	self.appsRoot = [[LCBrowserTreeApplicationsRoot new] autorelease];
	[newItems addObject:appsRoot];

	/* Faults */
	self.faultsRoot = [[LCBrowserTreeFaultsRoot new] autorelease];
	[newItems addObject:faultsRoot];
	
	/* Documents */
	self.documentsRoot = [[LCBrowserTreeDocumentsRoot new] autorelease];
	[newItems addObject:documentsRoot];

	/* Discovery */
	self.discoveryRoot = [[LCBrowserTreeDiscoveryRoot new] autorelease];
	[newItems addObject:discoveryRoot];
	
	/* Xsan */
	self.xsanRoot = [[LCBrowserTreeXsanRoot new] autorelease];
	[newItems addObject:xsanRoot];
	xsanShown = YES;
	
	/* Cores */
	self.coresRoot = [[LCBrowserTreeCoresRoot new] autorelease];
	[newItems addObject:coresRoot];
	
	/* Filter */
	NSPredicate *filterPred = [NSPredicate predicateWithFormat:@"enabled == YES"];
	[newItems filterUsingPredicate:filterPred];
	
	/* Sort */
	NSSortDescriptor *sortDesc = [[NSSortDescriptor alloc] initWithKey:@"preferredIndex" ascending:YES];
	[newItems sortUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
	[sortDesc autorelease];
	
	/* Set items */
	self.items = newItems;
	
	/* Expand all */
	[outlineView expandAllItemsUsingPreferences];
}

- (void) treeItemAdded:(NSNotification *)notification
{
	[outlineView expandAllItemsUsingPreferences];
}

- (void) rootItemEnabledChanged:(NSNotification *)note
{
	[self resetRootItems];
}

#pragma mark "Items Accessor"

@synthesize items;
- (void) setItems:(NSMutableArray *)value
{
	[items release];
	items = [value mutableCopy];
}

- (void) insertObject:(LCBrowserTreeItem *)item inItemsAtIndex:(unsigned int)index
{
	[items insertObject:item atIndex:index];
}
- (void) removeObjectFromItemsAtIndex:(unsigned int)index
{
	[items removeObjectAtIndex:index];
}

//#pragma mark "Customer Accessors" 
//
//- (LCCustomer *) customer
//{ return customer; }
//
//- (void) setCustomer:(LCCustomer *)newCustomer
//{
//	/* Remove old */
//	if (customer)
//	{
//		[customer removeObserver:self forKeyPath:@"xsanList.controllers"];
//		[customer removeObserver:self forKeyPath:@"xsanList.clients"];
//		[customer removeObserver:self forKeyPath:@"xsanList.volumes"];
//		[customer removeObserver:self forKeyPath:@"serviceList.services"];
//		[customer release];
//	}
//	
//	/* Set customer */
//	customer = [newCustomer retain];
//	
//	/* Add Observers */
//	[customer addObserver:self 
//			   forKeyPath:@"xsanList.controllers" 
//				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
//				  context:NULL];		
//	[customer addObserver:self 
//			   forKeyPath:@"xsanList.clients" 
//				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
//				  context:NULL];		
//	[customer addObserver:self 
//			   forKeyPath:@"xsanList.volumes" 
//				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
//				  context:NULL];		
//	[customer addObserver:self 
//			   forKeyPath:@"serviceList.services" 
//				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
//				  context:NULL];		
//	
//	
//	[self resetRootItems];
//}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"array"])
	{
		/* Change in customer list */
		
	}
//	if ([keyPath hasPrefix:@"xsanList"] && !xsanShown)
//	{ 
//		if ([[[customer xsanList] controllers] count] > 0 || 
//			[[[customer xsanList] volumes] count] > 0 ||
//			[[[customer xsanList] clients] count] > 0)
//		{ [self resetRootItems]; }
//	}	
//	if ([keyPath hasPrefix:@"serviceList"] && !servicesShown && [[[customer serviceList] services] count] > 0)
//	{ [self resetRootItems]; }	
}

#pragma mark "Properties"

@synthesize treeController;
@synthesize outlineView;
@synthesize xsanShown;
@synthesize servicesShown;
@synthesize devicesRoot;
@synthesize groupsRoot;
@synthesize documentsRoot;
@synthesize appsRoot;
@synthesize faultsRoot;
@synthesize discoveryRoot;
@synthesize coresRoot;
@synthesize xsanRoot;
@synthesize sortDescriptors;

@end
