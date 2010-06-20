//
//  LCBrowserActionsContentController.m
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserActionsContentController.h"
#import "LCActionEditWindowController.h"

@implementation LCBrowserActionsContentController

#pragma mark "Constructors"

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"ActionsContent" bundle:nil];
	if (!self) return nil;
	
	/* Set properties */
	self.resizeMode = RESIZE_TOP;
	self.customer = initCustomer;
	self.browser = initBrowser;
	self.actionList = [[[LCActionList alloc] initWithCustomer:initCustomer] autorelease];
	[self.actionList highPriorityRefresh];
	
	/* Load NIB */
	[self loadView];
	
	/* Observe Selection */
	[arrayController addObserver:self 
					  forKeyPath:@"selection" 
						 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
						 context:nil];
	
	/* Observe external changes */
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(actionListChanged:)
												 name:@"LCActionListChanged"
											   object:customer];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[arrayController removeObserver:self forKeyPath:@"selection"];
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:@"LCActionListChanged"
												  object:customer];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[customer release];
	[selectedActions release];
	[actionList release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedAction;
@synthesize selectedActions;
- (void) setSelectedActions:(NSArray *)value
{
	[selectedActions release];
	selectedActions = [value copy];
	
	if (selectedActions.count > 0) self.selectedAction = [selectedActions objectAtIndex:0];
	else self.selectedAction = nil;
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == arrayController)
	{
		self.selectedActions = [arrayController selectedObjects];
	}
}

- (void) actionListChanged:(NSNotification *)note
{
	[actionList highPriorityRefresh];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
//	SEL action = [item action];
	
	return NO;
}

#pragma mark "UI Actions"

- (IBAction) addNewClicked:(id)sender
{
	[LCActionEditWindowController beginSheetForNewAction:customer
										  windowForSheet:[browser window]
												delegate:self];
}

- (IBAction) editSelectedClicked:(id)sender
{
	[LCActionEditWindowController beginSheetForAction:self.selectedAction
									   windowForSheet:[browser window]
											 delegate:self];
}

- (IBAction) deleteSelectedClicked:(id)sender
{
	[LCActionEditWindowController beginSheetToDeleteAction:self.selectedAction
											windowForSheet:[browser window]
												  delegate:self];
}

- (IBAction) tableDoubleClicked:(NSArray *)tableSelectedObjects
{
	[LCActionEditWindowController beginSheetForAction:self.selectedAction
									   windowForSheet:[browser window]
											 delegate:self];
}

- (IBAction) refreshListClicked:(id)sender
{
	[actionList refreshWithPriority:XMLREQ_PRIO_HIGH];
}

#pragma mark "Action Edit Delegates"

- (void) actionEditDidAddAction:(LCAction *)delegateAction
{
	[self.actionList insertObject:delegateAction inObjectsAtIndex:self.actionList.objects.count];
}

- (void) actionEditDidUpdateAction:(LCAction *)delegateAction
{
	LCAction *localAction = [actionList.objectDict objectForKey:[NSString stringWithFormat:@"%i", delegateAction.taskID]];
	[localAction copyXmlPropertiesFromObject:delegateAction];
}

- (void) actionEditDidDeleteAction:(LCAction *)delegateAction
{
	[self.actionList removeObjectFromObjectsAtIndex:[self.actionList.objects indexOfObject:delegateAction]];
}

#pragma mark "Properties"

@synthesize actionList;
@synthesize browser;
@synthesize customer;

- (CGFloat) preferredFixedComponentHeight
{ return 186.0; }


@end
