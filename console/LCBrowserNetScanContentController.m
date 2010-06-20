//
//  LCBrowserNetScanContentController.m
//  Lithium Console
//
//  Created by James Wilson on 10/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserNetScanContentController.h"
#import "LCDeviceEditTemplate.h"
#import "LCDeviceEditController.h"


@implementation LCBrowserNetScanContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"NetScanContent" bundle:nil];
	if (self)
	{
		/* Setup */
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		
		/* Create Network Lists */
		networkLists = [[NSMutableArray array] retain];
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			LCIPRegistryNetworkList *networkList = [LCIPRegistryNetworkList networkListForCustomer:customer];
			[networkList setDelegate:self];
			[networkLists addObject:networkList];
			[networkList highPriorityRefresh];			
		}
		
		/* Load NIB */
		[self loadView];		
		outlineView.expansionPreferencePrefix = @"LCBrowserNetScanContentController";
		
		/* Force update of selection */
		[self observeValueForKeyPath:@"selection" ofObject:entryTreeController change:nil context:nil];
		
		/* Observe future change in selection */		
		[entryTreeController addObserver:self 
							  forKeyPath:@"selection" 
								 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								 context:nil];
		
	}
	return self;
}	

- (void) dealloc
{
	[entryTreeController removeObserver:self forKeyPath:@"selection"];
	[networkLists release];
	[selectedCustomer release];
	[selectedSite release];
	[selectedList release];
	[selectedEntry release];
	[selectedEntries release];
	[super dealloc];
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == entryTreeController)
	{
		/* Update selected customer */
		if ([[entryTreeController selectedObjects] count] > 0)
		{
			id selectedObject = [[entryTreeController selectedObjects] objectAtIndex:0];
			if ([selectedObject class] == [LCIPRegistryNetworkList class])
			{ 
				LCIPRegistryNetworkList *networkList = selectedObject;
				self.selectedList = selectedObject;
				self.selectedCustomer = networkList.customer; 
			}
			else if ([selectedObject class] == [LCIPRegistryNetwork class])
			{ 
				LCIPRegistryNetwork *selectedNetwork = selectedObject;
				self.selectedList = selectedNetwork.parent;
				self.selectedCustomer = [(LCIPRegistryNetworkList *)selectedNetwork.parent customer]; 
			}
			else if ([selectedObject class] == [LCIPRegistryEntry class])
			{ 
				LCIPRegistryEntry *entry = selectedObject;
				LCIPRegistryNetwork *selectedNetwork = entry.parent;
				self.selectedList = selectedNetwork.parent;
				self.selectedCustomer = [(LCIPRegistryNetworkList *)selectedNetwork.parent customer]; 
			}
			else
			{ 
				self.selectedCustomer = nil; 
				self.selectedList = nil;
			}
		}
		else
		{ 
			self.selectedCustomer = nil; 
			self.selectedList = nil;
		}
		
		/* Update service selection */
		NSMutableArray *treeSelectedEntries = [NSMutableArray array];;
		for (id obj in [entryTreeController selectedObjects])
		{
			if ([obj class] == [LCIPRegistryEntry class])
			{ [treeSelectedEntries addObject:obj]; }
		}
		if ([treeSelectedEntries count] > 0)
		{ 
			self.selectedEntry = [treeSelectedEntries objectAtIndex:0]; 
		}
		else
		{ 
			self.selectedEntry = nil; 
		}
		self.selectedEntries = treeSelectedEntries;		
	}
}

#pragma mark "Selection"

@synthesize selectedCustomer;
- (void) setSelectedCustomer:(LCCustomer *)value
{
	[selectedCustomer release];
	selectedCustomer = [value retain];
	
	if (selectedCustomer && [selectedCustomer.children count] > 0)
	{
		self.selectedSite = [selectedCustomer.children objectAtIndex:0];
	}
	else
	{
		self.selectedSite = nil;
	}
}
@synthesize selectedList;
@synthesize selectedSite;
@synthesize selectedEntry;
@synthesize selectedEntries;

#pragma mark "Network List Delegate Methods"

- (void) networkListRefreshFinished:(LCIPRegistryNetworkList *)networkList
{
	[outlineView expandAllItemsUsingPreferences];
}

#pragma mark "UI Actions"

- (IBAction) monitorDevicesClicked:(id)sender
{
	/* Add selected services as devices */
	NSMutableArray *templates = [NSMutableArray array];
	for (LCIPRegistryEntry *entry in selectedEntries)
	{
		LCDeviceEditTemplate *template = [LCDeviceEditTemplate new];
		if (entry.hostname)
		{
			template.name = entry.hostname;
			template.desc = entry.hostname;
		}
		else
		{
			template.name = entry.ip;
			template.desc = entry.ip;
		}
		template.ipAddress = entry.ip;
		[templates addObject:template];
		[template autorelease];
	}
	if (templates.count > 0)
	{
		/* Start Device Edit Controller */
		LCDeviceEditController *editController = [[LCDeviceEditController alloc] initForNewDevicesAtSite:self.selectedSite
																						  usingTemplates:templates
																							 usingModule:nil];
		[NSApp beginSheet:[editController window]
		   modalForWindow:[[self view] window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}	
}

- (IBAction) scanNetworkClicked:(id)sender
{
	[scanNetworkSheet makeFirstResponder:scanNetwork];
	[scanNetwork setStringValue:@""];
	[scanMask setStringValue:@"255.255.255.0"];
	[NSApp beginSheet:scanNetworkSheet 
	   modalForWindow:[[self view] window]
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) scanNetworkCancelClicked:(id)sender
{
	/* Close Sheet */
	[NSApp endSheet:scanNetworkSheet];
	[scanNetworkSheet close];
}

- (IBAction) scanNetworkScanClicked:(id)sender
{
	/* Scan */
	[self.selectedList scanNetwork:[scanNetwork stringValue] mask:[scanMask stringValue]];
	
	/* Close Sheet */
	[NSApp endSheet:scanNetworkSheet];
	[scanNetworkSheet close];	
}

#pragma mark "Properties"

@synthesize browser;


@end
