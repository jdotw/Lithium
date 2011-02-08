//
//  LCBrowserActionFilterContentController.m
//  Lithium Console
//
//  Created by James Wilson on 27/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LCBrowserActionFilterContentController.h"
#import "LCTransparentOutlineView.h"

@implementation LCBrowserActionFilterContentController

@synthesize browser, customer;

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"ActionFilterContents" bundle:nil];
	if (!self) return nil;
	
	/* Set properties */
	self.resizeMode = RESIZE_TOP;
	self.customer = initCustomer;
	self.browser = initBrowser;
	
	/* Load NIB */
	[self loadView];
	
	/* Observe Selection */
	[outlineView expandAllItemsUsingPreferences];
	[treeController addObserver:self 
					 forKeyPath:@"selection" 
						options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						context:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[treeController removeObserver:self forKeyPath:@"selection"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[selectedDevice release];
	[selectedDevices release];
	[selectedEntity release];
	[selectedEntities release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"
@synthesize selectedEntity;
- (void) setSelectedEntity:(LCEntity *)value
{
	[selectedEntity release];
	selectedEntity = [value retain];
	
	if (selectedEntity.type == 3) self.selectedDevice = (LCDevice *)selectedEntity;
	else self.selectedDevice = nil;
}
@synthesize selectedEntities;
- (void) setSelectedEntities:(NSArray *)value
{
	[selectedEntities release];
	selectedEntities = [value copy];
	
	NSMutableArray *devices = [NSMutableArray array];
	for (LCEntity *entity in selectedEntities)
	{
		if (entity.type == 3) [devices addObject:entity];
	}
	self.selectedDevices = devices;
	
	if (selectedEntities.count == 1) self.selectedDevice = [selectedEntities objectAtIndex:0];
	else self.selectedDevice = nil;
}
@synthesize selectedDevice, selectedDevices;

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == treeController)
	{
		self.selectedEntities = [treeController selectedObjects];
	}
}

#pragma mark "Properties"

- (CGFloat) preferredFixedComponentHeight
{ return 186.0; }

@end
