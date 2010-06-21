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
@property (nonatomic, copy) NSMutableArray *items;
@property (nonatomic,retain) NSTreeController *treeController;
@property (nonatomic, assign) LCBrowserTreeOutlineView *outlineView;
@property BOOL xsanShown;
@property BOOL servicesShown;
@property (nonatomic,retain) LCBrowserTreeDevicesRoot *devicesRoot;
@property (nonatomic,retain) LCBrowserTreeGroupsRoot *groupsRoot;
@property (nonatomic,retain) LCBrowserTreeApplicationsRoot *appsRoot;
@property (nonatomic,retain) LCBrowserTreeDocumentsRoot *documentsRoot;
@property (nonatomic,retain) LCBrowserTreeFaultsRoot *faultsRoot;
@property (nonatomic,retain) LCBrowserTreeDiscoveryRoot *discoveryRoot;
@property (nonatomic,retain) LCBrowserTreeXsanRoot *xsanRoot;
@property (nonatomic,retain) LCBrowserTreeCoresRoot *coresRoot;
@property (nonatomic,copy) NSArray *sortDescriptors;

@end
