//
//  LCBrowserApplicationContentController.m
//  Lithium Console
//
//  Created by James Wilson on 16/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserApplicationContentController.h"

#import "LCObjectTree.h"
#import "LCMetricGraphDocument.h"
#import "LCFaultHistoryController.h"
#import "LCPropertiesViewerController.h"
#import "LCMetricHistoryWindowController.h"
#import "LCTriggerTuningWindowController.h"
#import "LCMetricAnalysisWindowController.h"
#import "LCCaseController.h"
#import "LCServiceEditWindowController.h"
#import "LCProcessProfileEditWindowController.h"
#import "LCDeviceEditController.h"


@implementation LCBrowserApplicationContentController

- (id) initWithBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [self initWithNibName:@"ApplicationContent" bundle:nil];
	if (!self) return nil;
	
	/* Set/Create objects */
	objects = [[NSMutableArray array] retain];
	self.browser = initBrowser;
	
	/* Load the NIB */
	[self loadView];
	
	/* Inspector setup */
	[inspectorController bind:@"target" toObject:self withKeyPath:@"selectedEntity" options:nil];
	
	/* Observe Selection */
	[objectTreeController addObserver:self 
						   forKeyPath:@"selection" 
							  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
							  context:nil];
	[stadiumController addObserver:self
						forKeyPath:@"selectedEntity"
						   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						   context:nil];
	
	/* Observe change in Container and Object OutlineView Responder */
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(firstResponderChanged:) 
												 name:@"WillBecomeFirstResponder" 
											   object:objectOutlineView];
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(firstResponderChanged:) 
												 name:@"WillBecomeFirstResponder" 
											   object:browser.browserTreeOutlineView];
	

	return self;	
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[inspectorController unbind:@"target"];
	[objectTreeController removeObserver:self forKeyPath:@"selection"];
	[stadiumController removeObserver:self forKeyPath:@"selectedEntity"];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"WillBecomeFirstResponder" object:objectOutlineView];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:@"WillBecomeFirstResponder" object:browser.browserTreeOutlineView];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[objects release];	
	[device release];
	[objectTree release];
	[selectedEntities release];
	
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

- (void) updateEntitySelectionUsingObjectTree
{
	/* Container Tree Selection Changed */
	if ([[objectTreeController selectedObjects] count] > 0)
	{
		/* Set selection */
		NSMutableArray *selectedObjects = [NSMutableArray array];
		for (LCObjectTreeItem *item in [objectTreeController selectedObjects])
		{ [selectedObjects addObject:item.object]; }
		self.selectedEntities = selectedObjects; 
	}	
	else
	{
		if ([self.window firstResponder] == objectOutlineView)
		{ self.selectedEntities = nil; }
	}	
}

- (void) updateEntitySelectionUsingGraphStadium
{
	if (stadiumController.selectedEntity)
	{
		if ([stadiumController.selectedEntity metric])
		{ self.selectedEntities = [NSArray arrayWithObject:[stadiumController.selectedEntity object]]; }
		else
		{ self.selectedEntities = [NSArray arrayWithObject:stadiumController.selectedEntity]; }
	}
	else
	{
		self.selectedEntities = nil;
	}
}

@synthesize selectedEntity;

@synthesize selectedEntities;
- (void) setSelectedEntities:(NSArray *)value
{
	[selectedEntities release];
	selectedEntities = [value copy];
	
	/* Check to see if a selection was specified */
	if ([selectedEntities count] > 0)
	{ 
		/* Selection specified, use it. */
		self.selectedEntity = [selectedEntities objectAtIndex:0];
		
		/* Update stadium flow */
		if (!doNotScrollFlow)
		{
			if (stadiumController.flowController.target != [self.selectedEntity container])
			{ stadiumController.flowController.target = [self.selectedEntity container]; }
			if ([self.selectedEntity object])
			{ [stadiumController.flowController scrollToObject:[self.selectedEntity object]]; }
		}
	}
	else
	{ 
		/* No selecton specified, use device */
		self.selectedEntity = nil;
		stadiumController.flowController.target = nil;
	}
}

- (void) selectEntity:(LCEntity *)entity
{
	[objectOutlineView selectObject:[entity object]];
	[[self window] makeFirstResponder:objectOutlineView];
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == objectTreeController)
	{
		[self updateEntitySelectionUsingObjectTree];
	}
	else if (object == stadiumController)
	{
		LCObject *selectedObject = (LCObject *) stadiumController.selectedEntity;
		if (selectedObject)
		{ 
			[objectOutlineView selectObject:selectedObject.object];
			[[self window] makeFirstResponder:objectOutlineView];
		}
		[self updateEntitySelectionUsingGraphStadium];
	}
}

- (void) firstResponderChanged:(NSNotification *)note
{
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	/* 
	 * Monitored Entity
	 */
	
	if (action == @selector(graphSelectedClicked:)) 
	{ if ([selectedEntity container]) return YES; }
	
	else if (action == @selector(faultHistoryClicked:)) 
	{ if ([selectedEntity customer]) return YES; }
	
	else if (action == @selector(analyseSelectedClicked:)) 
	{ if ([selectedEntity object]) return YES; }
	
	else if (action == @selector(openCaseForSelectedClicked:))
	{ if ([selectedEntity customer] && [[selectedEntity customer] userIsNormal] && [self.selectedEntities count] > 0) return YES; }
	
	else if (action == @selector(triggerTuningClicked:))
	{ if ([selectedEntity object] && [[selectedEntity customer] userIsAdmin] && [self.selectedEntity object]) return YES; }
	
	else if (action == @selector(copyWebUrlToClipBoardClicked:))
	{ if ([selectedEntity customer]) return YES; }
	
	else if (action == @selector(openWebUrlInBrowserClicked:))
	{ if ([selectedEntity customer]) return YES; }
	
	else if (action == @selector(browseToSelectedClicked:))
	{ if (selectedEntity) return YES; }
	
	else if (action == @selector(faultHistoryClicked:))
	{ if (selectedEntity) return YES; }
		
	return NO;
}

#pragma mark "UI Actions"

- (IBAction) graphSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, and graph them */
	LCMetricGraphDocument *document = [[NSDocumentController sharedDocumentController] makeUntitledDocumentOfType:@"LCMetricGraphDocument"];
	document.initialEntities = self.selectedEntities;
	[[NSDocumentController sharedDocumentController] addDocument:document];
	[document makeWindowControllers];
	[document showWindows];			
}

- (IBAction) faultHistoryClicked:(NSMenuItem *)sender
{
	/* Create fault history controller */
	for (LCEntity *entity in self.selectedEntities)
	{ [[LCFaultHistoryController alloc] initForEntity:entity]; }
}

- (IBAction) openCaseForSelectedClicked:(NSMenuItem *)sender
{
	/* Open Case */
	[[LCCaseController alloc] initForNewCaseWithEntityList:self.selectedEntities];
}

- (IBAction) viewPropertiesClicked:(NSMenuItem *)sender
{
	/* Check entities */
	for (LCEntity *entity in self.selectedEntities)
	{ [[LCPropertiesViewerController alloc] initWithDictionary:[entity properties]]; }
}

- (IBAction) metricHistoryClicked:(id)sender
{
	for (LCEntity *entity in self.selectedEntities)
	{
		if ([[entity typeInteger] intValue] != 6)
		{ continue; }
		[[LCMetricHistoryWindowController alloc] initWithMetric:(LCMetric *)entity];
	}
}

- (IBAction) triggerTuningClicked:(id)sender
{
	/* Open trigger tuning window */
	LCTriggerTuningWindowController *controller = (LCTriggerTuningWindowController *) [[LCTriggerTuningWindowController alloc] initWithObject:[self.selectedEntity object]];
	[NSApp beginSheet:[controller window]
	   modalForWindow:[self window] 
		modalDelegate:self 
	   didEndSelector:nil 
		  contextInfo:nil];	
}

- (IBAction) analyseSelectedClicked:(id)sender
{
	/* Open analysis window */
	[[LCMetricAnalysisWindowController alloc] initWithObject:[self.selectedEntity object]];
}

- (IBAction) copyWebUrlToClipBoardClicked:(id)sender
{
	/* Copy */
	if ([self.selectedEntity webURLString])
	{
		NSPasteboard *pb = [NSPasteboard generalPasteboard];
		NSArray *types = [NSArray arrayWithObject:NSStringPboardType];
		[pb declareTypes:types owner:self];
		[pb setString:[self.selectedEntity webURLString] forType:NSStringPboardType];
	}
}

- (IBAction) openWebUrlInBrowserClicked:(id)sender
{
	/* Open */
	if ([self.selectedEntity webURLString])
	{
		NSURL *url = [NSURL URLWithString:[self.selectedEntity webURLString]];
		[[NSWorkspace sharedWorkspace] openURL:url];
	}
}

- (IBAction) browseToSelectedClicked:(NSMenuItem *)sender
{
	/* Get the current selected entities, open a browser for the first one */
	if (self.selectedEntity)
 	{ [[[LCBrowser2Controller alloc] initWithEntity:self.selectedEntity] autorelease]; }
}

#pragma mark "Properties"

@synthesize objects;
- (void) insertObject:(id)obj inObjectsAtIndex:(unsigned int)index
{
	[objects insertObject:obj atIndex:index];
	stadiumController.targetArray = objects;
}
- (void) removeObjectFromObjectsAtIndex:(unsigned int)index
{
	[objects removeObjectAtIndex:index];
	stadiumController.targetArray = objects;
}
@synthesize browser;
@synthesize objectTree;
- (NSWindow *) window
{ 
	NSResponder *nr = self;
	while ((nr = [nr nextResponder]))
	{ if ([[nr class] isSubclassOfClass:[NSWindow class]]) return (NSWindow *) nr; }
	return nil;
}
- (CGFloat) preferredFixedComponentHeight
{ return 320.0; }

@synthesize doNotScrollFlow;

@synthesize stadiumTitle;

@end
