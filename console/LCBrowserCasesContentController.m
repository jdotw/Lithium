//
//  LCBrowserCasesContentController.m
//  Lithium Console
//
//  Created by James Wilson on 27/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserCasesContentController.h"

#import "LCCaseController.h"
#import "LCCaseLogUpdateController.h"

@implementation LCBrowserCasesContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"CasesContent" bundle:nil];
	if (self)
	{
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		[self loadView];
		entitySummaryTableView.roundedSelection = YES;
		
		/* Force update of selection */
		[self observeValueForKeyPath:@"selection" ofObject:caseArrayController change:nil context:nil];
		
		/* Observe future change in selection */		
		[caseArrayController addObserver:self 
							  forKeyPath:@"selection" 
								 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								 context:nil];
		
		/* Set Inpector Binding */
		[inspectorController bind:@"target" toObject:self withKeyPath:@"selectedCase" options:nil];
	}
	return self;
}	

- (void) dealloc
{
	[caseArrayController removeObserver:self forKeyPath:@"selection"];
	[inspectorController unbind:@"target"];
	[super dealloc];
}

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == caseArrayController)
	{
		/* Update selection properties */
		if ([[caseArrayController selectedObjects] count] > 0)
		{ 
			self.selectedCase = [[caseArrayController selectedObjects] objectAtIndex:0]; 
			[[self.selectedCase logEntryList] highPriorityRefresh];
		}
		else
		{ self.selectedCase = 0; }
		self.selectedCases = [caseArrayController selectedObjects];
		
	}
}

#pragma mark "Selection"

@synthesize selectedCase;
@synthesize selectedCases;

#pragma mark "UI Actions"

- (void) caseTableViewDoubleClick:(NSArray *)doubleClickedCases
{
	/* Called when there is a doubleclick on the case tableview */
	
	for (LCCase *clickedCase in doubleClickedCases)
	{ [[LCCaseController alloc] initWithCase:clickedCase]; }
	
}

#pragma mark "Case Action Methods"

- (IBAction) viewCaseClicked:(id)sender
{
	for (LCCase *cas in selectedCases)
	{ [[LCCaseController alloc] initWithCase:cas]; }
}

- (IBAction) newCaseClicked:(id)sender
{
	[[LCCaseController alloc] initForNewCase];
}

- (IBAction) closeCaseClicked:(id)sender
{
	/* Close the selected cases */
	
	/* Check cases are selected */
	if (selectedCases.count < 1) return;
	
	/* Call updateSelectedCases to close */
	[[LCCaseLogUpdateController alloc] initWithCase:selectedCase closeCase:YES];
}

- (IBAction) updateLogClicked:(id)sender
{
	/* Update the selected cases */
	
	/* Check cases are selected */
	if (selectedCases.count < 1) return;
	
	/* Call updateSelectedCases to update */
	[[LCCaseLogUpdateController alloc] initWithCase:selectedCase closeCase:NO];
}

- (IBAction) addNewCasePopupClicked:(id)sender
{
	/* Clear old */
	while ([[openNewCaseMenu itemArray] count] > 0) 
	{ [openNewCaseMenu removeItemAtIndex:0]; }
	
	/* Construct Site Menu */
	if ([[LCCustomerList masterArray] count] > 1)
	{ 
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			/* Add item for each customer */
			NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:customer.displayString
														  action:@selector(addNewCaseForCustomerMenuClicked:) 
												   keyEquivalent:@""];
			[item setTarget:self];
			[item setRepresentedObject:customer];
			[openNewCaseMenu insertItem:item atIndex:[[openNewCaseMenu itemArray] count]];			
			[item autorelease];
		}
	}	
	
	/* Pop menu */
	[NSMenu popUpContextMenu:openNewCaseMenu withEvent:[NSApp currentEvent] forView:openNewCaseButton];
}

- (IBAction) addNewCaseForCustomerMenuClicked:(NSMenuItem *)sender
{
	LCCustomer *customer = [sender representedObject];
	[[LCCaseController alloc] initForNewCaseAtCustomer:customer];
}

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	/* Obtain selection */
	SEL action = [item action];
	
	/* Reference Customer */
	LCCustomer *referenceCustomer = nil;
	if (selectedCase)
	{ referenceCustomer = [selectedCase customer]; }
	
	/* Toolbar */
	
	/*
	 * Case
	 */
	
	if (action == @selector(viewCaseClicked:))
	{ if (selectedCase) return YES; }
	
	if (action == @selector(closeCaseClicked:))
	{ if (selectedCase && [referenceCustomer userIsNormal]) return YES; }
	
	if (action == @selector(newCaseClicked:))
	{ if (selectedCase && [referenceCustomer userIsNormal]) return YES; }

	if (action == @selector(updateLogClicked:))
	{ if (selectedCase && [referenceCustomer userIsNormal]) return YES; }
	
	if (action == @selector(addNewCaseForCustomerMenuClicked:)) return YES;
	
	return NO;
}

#pragma mark "Properties"

- (CGFloat) preferredFixedComponentHeight
{ return 200.0; }

- (CGFloat) preferredInspectorWidth
{ return 210.0; }

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@synthesize browser;

@end
