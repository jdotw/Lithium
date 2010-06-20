//
//  LCBrowserBonjourContentController.m
//  Lithium Console
//
//  Created by James Wilson on 9/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserBonjourContentController.h"
#import "LCDeviceEditTemplate.h"
#import "LCDeviceEditController.h"

@implementation LCBrowserBonjourContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"BonjourContent" bundle:nil];
	if (self)
	{
		/* Setup */
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		
		/* Create Browser Lists */
		browserLists = [[NSMutableArray array] retain];
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			LCBonjourBrowserList *browserList = [LCBonjourBrowserList bonjourBrowserListForCustomer:customer];
			browserList.delegate = self;
			[browserLists addObject:browserList];
			[browserList highPriorityRefresh];
		}
		
		/* Load NIB */
		[self loadView];
		outlineView.expansionPreferencePrefix = @"LCBrowserBonjourContentController";
		
		/* Force update of selection */
		[self observeValueForKeyPath:@"selection" ofObject:serviceTreeController change:nil context:nil];
		
		/* Observe future change in selection */		
		[serviceTreeController addObserver:self 
								forKeyPath:@"selection" 
								   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								   context:nil];
		
	}
	return self;
}	

- (void) dealloc
{
	[serviceTreeController removeObserver:self forKeyPath:@"selection"];
	[browserLists release];
	[selectedCustomer release];
	[selectedSite release];
	[selectedService release];
	[selectedServices release];
	[super dealloc];
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == serviceTreeController)
	{
		/* Update selected customer */
		if ([[serviceTreeController selectedObjects] count] > 0)
		{
			id selectedObject = [[serviceTreeController selectedObjects] objectAtIndex:0];
			if ([selectedObject class] == [LCBonjourBrowserList class])
			{ 
				LCBonjourBrowserList *browserList = selectedObject;
				self.selectedCustomer = browserList.customer; 
			}
			else if ([selectedObject class] == [LCBonjourBrowser class])
			{ 
				LCBonjourBrowser *selectedBrowser = selectedObject;
				self.selectedCustomer = [(LCBonjourBrowserList *)selectedBrowser.parent customer]; 
			}
			else if ([selectedObject class] == [LCBonjourService class])
			{ 
				LCBonjourService *service = selectedObject;
				LCBonjourBrowser *selectedBrowser = [service parent];				
				self.selectedCustomer = [(LCBonjourBrowserList *)selectedBrowser.parent customer]; 
			}
			else
			{ self.selectedCustomer = nil; }
		}
		else
		{ self.selectedCustomer = nil; }
		
		/* Update service selection */
		NSMutableArray *treeSelectedServices = [NSMutableArray array];;
		for (id obj in [serviceTreeController selectedObjects])
		{
			if ([obj class] == [LCBonjourService class])
			{ [treeSelectedServices addObject:obj]; }
		}
		if ([treeSelectedServices count] > 0)
		{ 
			self.selectedService = [treeSelectedServices objectAtIndex:0]; 
		}
		else
		{ 
			self.selectedService = nil; 
		}
		self.selectedServices = treeSelectedServices;		
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
@synthesize selectedSite;
@synthesize selectedService;
@synthesize selectedServices;

#pragma mark "Bonjour List Delegate"

- (void) bonjourListRefreshFinished:(LCBonjourBrowserList *)browserList
{
	[outlineView expandAllItemsUsingPreferences];
}

#pragma mark "UI Actions"

- (IBAction) monitorDevicesClicked:(id)sender
{
	/* Add selected services as devices */
	NSMutableArray *templates = [NSMutableArray array];
	for (LCBonjourService *service in selectedServices)
	{
		LCDeviceEditTemplate *template = [LCDeviceEditTemplate new];
		template.name = service.name;
		template.desc = service.name;
		template.ipAddress = service.ip;
		[templates addObject:template];
		[template autorelease];
	}
	if (templates.count > 0)
	{
		/* Determine Module to use */
		LCBonjourBrowser *selectedBrowser = selectedService.parent;
		NSString *module = nil;
		if ([selectedBrowser.service isEqualToString:@"_servermgr._tcp"]) module = @"osx_server";
		else if ([selectedBrowser.service isEqualToString:@"_airport._tcp"]) module = @"airport";
		else if ([selectedBrowser.service isEqualToString:@"_xserveraid._tcp"]) module = @"xraid";
		
		/* Start Device Edit Controller */
		LCDeviceEditController *editController = [[LCDeviceEditController alloc] initForNewDevicesAtSite:self.selectedSite
																						  usingTemplates:templates
																							 usingModule:module];
		[NSApp beginSheet:[editController window]
		   modalForWindow:[[self view] window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}	
}

#pragma mark "Properties"

@synthesize browser;

@end
