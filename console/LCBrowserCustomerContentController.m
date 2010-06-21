//
//  LCBrowserCustomerContentController.m
//  Lithium Console
//
//  Created by James Wilson on 5/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserCustomerContentController.h"

#import "LCMetricGraphDocument.h"
#import "LCFaultHistoryController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCCaseController.h"
#import "LCDeviceTreeItem.h"
#import "LCDeviceTreeDeviceItem.h"
#import "LCDeviceTreeIncidentItem.h"

@implementation LCBrowserCustomerContentController

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [self initWithNibName:@"CustomerContent" bundle:nil];
	
	if (self)
	{
		/* Set/Create objects */
		self.browser = initBrowser;
		self.customer = initCustomer;
		[self.customer highPriorityRefresh];
		NSTimeInterval autoRefreshInterval = 30.0;
		refreshTimer = [[NSTimer scheduledTimerWithTimeInterval:autoRefreshInterval
														 target:self 
													   selector:@selector(refreshTimerFired:) 
													   userInfo:nil 
														repeats:YES] retain];
		
		/* Create device tree */
		self.deviceTree = [[[LCDeviceTree alloc] initWithCustomer:customer] autorelease];
		
		/* Set initial selection */
		self.selectedDevices = [NSArray arrayWithObject:self.customer];
		self.inspectorTarget = self.customer;
		
		/* Load NIB */
		[self loadView];
		[deviceOutlineView expandAllItemsUsingPreferences];
		
		/* Inspector setup */
		[inspectorController bind:@"target" toObject:self withKeyPath:@"inspectorTarget" options:nil];
		
		/* Observe Device Tree Selection */
		[deviceTreeController addObserver:self 
							   forKeyPath:@"selection" 
								  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								  context:nil];		
	}
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[inspectorController unbind:@"target"];
	[deviceTreeController removeObserver:self forKeyPath:@"selection"];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"EntityRefreshFinished" object:customer];
	[refreshTimer invalidate];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[deviceTree release];
	[refreshTimer release];
	[selectedDevices release];
	[selectedIncidents release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedDevice;

@synthesize selectedDevices;
- (void) setSelectedDevices:(NSArray *)value
{
	[selectedDevices release];
	selectedDevices = [value copy];
	
	/* Check to see if a selection was specified */
	if ([selectedDevices count] > 0)
	{ 
		/* Selection specified, use it. */
		self.selectedDevice = [selectedDevices objectAtIndex:0];		
	}
	else
	{ 
		/* No selecton specified, use site */
		self.selectedDevice = [NSArray arrayWithObject:self.customer];
	}
}

@synthesize selectedIncident;
@synthesize selectedIncidents;
- (void) setSelectedIncidents:(NSArray *)value
{
	[selectedIncidents release];
	selectedIncidents = [value copy];
	
	/* Check to see if a selection was specified */
	if ([selectedIncidents count] > 0)
	{ 
		/* Selection specified, use it. */
		self.selectedIncident = [selectedIncidents objectAtIndex:0];		
	}
	else
	{ 
		/* No selecton specified */
		self.selectedIncident = nil;
	}
}
@synthesize inspectorTarget;	

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == deviceTreeController)
	{
		/* Device Tree Selection Changed */
		if ([[deviceTreeController selectedObjects] count] > 0)
		{
			/* Set selection */
			NSMutableArray *devices = [NSMutableArray array];
			NSMutableArray *incidents = [NSMutableArray array];
			for (LCDeviceTreeItem *item in [deviceTreeController selectedObjects])
			{ 
				if ([[item class] isSubclassOfClass:[LCDeviceTreeDeviceItem class]])
				{ 
					LCDeviceTreeDeviceItem *devItem = (LCDeviceTreeDeviceItem *) item;
					[devices addObject:devItem.device];
				}
				if ([[item class] isSubclassOfClass:[LCDeviceTreeIncidentItem class]])
				{ 
					LCDeviceTreeIncidentItem *incItem = (LCDeviceTreeIncidentItem *) item;
					[incidents addObject:incItem.incident];
				}
			}
			if (incidents.count > 0) self.inspectorTarget = [incidents objectAtIndex:0];
			else if (devices.count > 0) self.inspectorTarget = [devices objectAtIndex:0];
			else self.inspectorTarget = customer;
			self.selectedDevices = devices;
			self.selectedIncidents = incidents;
		}	
		else
		{
			self.selectedDevices = nil;		/* Will use site */
			self.selectedIncidents = nil;
			self.inspectorTarget = customer;
		}	
	}
}

#pragma mark "UI Actions"

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, and graph them */
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = [NSArray arrayWithObject:self.selectedIncident.entity];
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (IBAction) faultHistoryClicked:(NSMenuItem *)sender
{
	/* Create fault history controller */
	for (LCIncident *incident in self.selectedIncidents)
	{ [[LCFaultHistoryController alloc] initForEntity:incident.entity]; }
}

- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender
{
	/* Open Case */
	if ([self.selectedIncidents count] > 0)
	{
		[[LCCaseController alloc] initForNewCaseWithEntityList:[self valueForKeyPath:@"selectedIncidents.entity"]];
	}
	else if ([self.selectedIncidents count] > 0)
	{
		[[LCCaseController alloc] initForNewCaseWithEntityList:self.selectedDevices];
	}	
}

- (IBAction) metricHistoryClicked:(id)sender
{
	for (LCIncident *incident in self.selectedIncidents)
	{
		if (incident.entity.metric)
		{ [[LCMetricHistoryWindowController alloc] initWithMetric:(LCMetric *)incident.entity.metric]; }
	}
}

- (IBAction) triggerTuningClicked:(id)sender
{
	/* Open trigger tuning window */
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[self.selectedIncident.entity object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) analyseSelectedClicked:(id)sender
{
	/* Open analysis window */
	[[LCMetricAnalysisWindowController alloc] initWithObject:[self.selectedIncident.entity object]];
}

- (IBAction) copyWebUrlToClipBoardClicked:(id)sender
{
	/* Copy */
	NSString *webURLString;
	if (self.selectedIncident) webURLString = [[self.selectedIncident.entity object] webURLString];
	else webURLString = [self.selectedDevice webURLString];
	if (webURLString)
	{
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		NSArray *types = [NSArray arrayWithObject:NSStringPboardType];
		[pb declareTypes:types owner:self];
		[pb setString:webURLString forType:NSStringPboardType];
	}
}

- (IBAction) openWebUrlInBrowserClicked:(id)sender
{
	/* Open */
	NSString *webURLString;
	if (self.selectedIncident) webURLString = [[self.selectedIncident.entity object] webURLString];
	else webURLString = [self.selectedDevice webURLString];
	if (webURLString)
	{
		NSURL *url = [NSURL URLWithString:webURLString];
		[[NSWorkspace sharedWorkspace] openURL:url];
	}
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, open a browser for the first one */
	if (self.selectedIncident)
 	{ [[[LCBrowser2Controller alloc] initWithEntity:self.selectedIncident.entity] autorelease]; }
	else
 	{ [[[LCBrowser2Controller alloc] initWithEntity:self.selectedDevice] autorelease]; }
}

- (IBAction) deviceOutlineViewDoubleClicked:(NSArray *)selectedItems
{
	if (selectedItems.count > 0)
	{
		id item = [selectedItems objectAtIndex:0];
		if ([[item class] isSubclassOfClass:[LCDeviceTreeDeviceItem class]])
		{
			/* Device double clicked */
			LCDeviceTreeDeviceItem *devItem = (LCDeviceTreeDeviceItem *) item;
			[browser selectEntity:devItem.device];
		}
		else if ([[item class] isSubclassOfClass:[LCDeviceTreeIncidentItem class]])
		{
			/* Incident double clicked */
			LCDeviceTreeIncidentItem *incItem = (LCDeviceTreeIncidentItem *) item;
			[browser selectIncident:incItem.incident];
		}
	}			
}

#pragma mark "Device Refresh"

- (void) refreshTimerFired:(NSTimer *)timer
{
	[self.customer normalPriorityRefresh];
}

#pragma mark "Properties"

@synthesize browser;
@synthesize customer;
@synthesize deviceTree;
- (NSWindow *) window
{ 
	NSResponder *nr = self;
	while ((nr = [nr nextResponder]))
	{ if ([[nr class] isSubclassOfClass:[NSWindow class]]) return (NSWindow *) nr; }
	return nil;
}
- (CGFloat) preferredFixedComponentHeight
{ return 320.0; }



@end
