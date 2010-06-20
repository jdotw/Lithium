//
//  LCBrowserTree.h
//  Lithium Console
//
//  Created by James Wilson on 30/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"
#import "LCBrowserTreeItem.h"
#import "LCBrowserTreeOutlineView.h"
#import "LCBrowserTreeDocumentsRoot.h"
#import "LCBrowserTreeFaultsRoot.h"
#import "LCBrowserTreeDevicesRoot.h"
#import "LCBrowserTreeGroupsRoot.h"
#import "LCBrowserTreeApplicationsRoot.h"
#import "LCBrowserTreeDiscoveryRoot.h"
#import "LCBrowserTreeXsanRoot.h"
#import "LCBrowserTreeCoresRoot.h"

@interface LCBrowserTree : NSObject 
{
	/* Items */
	NSMutableArray *items;
	
	/* Tree Controller */
	NSTreeController *treeController;
	
	/* OutlineView */
	LCBrowserTreeOutlineView *outlineView;
	
	/* Optional Items */
	BOOL xsanShown;
	BOOL servicesShown;
	
	/* Root Items */
	LCBrowserTreeDevicesRoot *devicesRoot;
	LCBrowserTreeGroupsRoot *groupsRoot;
	LCBrowserTreeApplicationsRoot *appsRoot;
	LCBrowserTreeDocumentsRoot *documentsRoot;
	LCBrowserTreeFaultsRoot *faultsRoot;
	LCBrowserTreeDiscoveryRoot *discoveryRoot;
	LCBrowserTreeXsanRoot *xsanRoot;
	LCBrowserTreeCoresRoot *coresRoot;
	
	/* Sort */
	NSArray *sortDescriptors;
}

#pragma mark "Constructor"
+ (LCBrowserTree *) browserTree;
- (LCBrowserTree *) init;
- (void) dealloc;

#pragma mark "Root Item Management"
- (void) resetRootItems;
- (void) insertObject:(LCBrowserTreeItem *)item inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;
@property (copy) NSMutableArray *items;
@property (retain) NSTreeController *treeController;
@property (assign) LCBrowserTreeOutlineView *outlineView;
@property BOOL xsanShown;
@property BOOL servicesShown;
@property (retain) LCBrowserTreeDevicesRoot *devicesRoot;
@property (retain) LCBrowserTreeGroupsRoot *groupsRoot;
@property (retain) LCBrowserTreeApplicationsRoot *appsRoot;
@property (retain) LCBrowserTreeDocumentsRoot *documentsRoot;
@property (retain) LCBrowserTreeFaultsRoot *faultsRoot;
@property (retain) LCBrowserTreeDiscoveryRoot *discoveryRoot;
@property (retain) LCBrowserTreeXsanRoot *xsanRoot;
@property (retain) LCBrowserTreeCoresRoot *coresRoot;
@property (copy) NSArray *sortDescriptors;

@end
