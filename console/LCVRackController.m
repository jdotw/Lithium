//
//  LCVRackController.m
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCVRackController.h"

#import "LCVRackView.h"
#import "LCVRackDeviceBackView.h"
#import "LCVRackDeviceFrontView.h"
#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"

@implementation LCVRackController

#pragma mark Awake From NIB

- (void) dealloc
{
	[cableListSearchString release];
	[cableListFilterPredicate release];
	[devFrontViews release];
	[devBackViews release];
	[super dealloc];
}

#pragma mark Shutdown

- (void) shutdown
{
	[self setCableEndPointA:nil];
	[self setHoverObject:nil];
	[self setHoverEntity:nil];
	[self setHoverDevice:nil];
	[self setHoverCable:nil];
	[self setSelectedCable:nil];
	[self setSelectedDevice:nil];

	/* Clear old front views */
	for (LCVRackDeviceFrontView *devView in devFrontViews)
	{ [devView removeViewAndContent]; }
	[devFrontViews removeAllObjects];
	
	/* Clear out old back views */
	for (LCVRackDeviceBackView *devView in devBackViews)
	{ [devView removeViewAndContent]; }
	[devBackViews removeAllObjects];	
	
	/* Kill cableView */
	[cableArrayController removeObserver:self forKeyPath:@"selection"];
	[cableView setRackController:nil];
	[cableView removeFromSuperview];
	[cableView release];
	cableView = nil;
	
	/* Set rack to nil */
	[self setRack:nil];
}

#pragma mark Rack Methods

- (LCVRackDocument *) rack
{ return rack; }
- (void) setRack:(LCVRackDocument *)newRack
{
	/* Remove old */
	if (rack)
	{
		[rack removeObserver:self forKeyPath:@"devices"];
		[rack removeObserver:self forKeyPath:@"cables"];
		[rack release];
	}
	
	/* Set new */
	rack = [newRack retain];

	if (rack)
	{
		/* Add observers */
		[rack addObserver:self
			   forKeyPath:@"devices" 
				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				  context:NULL];
		[rack addObserver:self 
			   forKeyPath:@"cables" 
				  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				  context:NULL];

		/* Re-display racks */
		[frontView setNeedsDisplay:YES];
		[backView setNeedsDisplay:YES];
		[cableView setPathsValid:NO];
		[cableView setNeedsDisplay:YES];
	}
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"devices"] || [keyPath isEqualToString:@"cables"])
	{
		[self resetDeviceViews];
		[self resetCableViews];
	}
	else if ([keyPath isEqualToString:@"selection"] && object == cableArrayController)
	{
		if ([[cableArrayController selectedObjects] count] > 0)
		{
			[self setHoverCable:[[cableArrayController selectedObjects] objectAtIndex:0]];
			[self setSelectedCable:[[cableArrayController selectedObjects] objectAtIndex:0]];
		}
		else
		{
			[self setHoverCable:nil]; 
			[self setSelectedCable:nil];
		}
	}
}

#pragma mark "Hover Entity Methods"

- (LCEntity *) hoverEntity
{ return hoverEntity; }

- (void) setHoverEntity:(LCEntity *)newEntity
{
	if (hoverEntity) [hoverEntity release];
	hoverEntity = [newEntity retain];
}

- (int) hoverObjectType
{
	return hoverObjectType; 
}

- (void) setHoverObjectType:(int)type
{
	hoverObjectType = type;
}

- (id) hoverObject
{
	return hoverObject;
}

- (void) setHoverObject:(id)newObject
{
	if (hoverObject) [hoverObject release];
	hoverObject = [newObject retain];
}

#pragma mark "Cable Methods"

- (void) resetCableViews
{
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];	
}

- (void) resizeCableViews
{
	[cableView setFrame:[backView rectForRack]];	
	[cableView setPathsValid:NO];
}

- (LCVRackCableView *) cableView
{ return cableView; }

- (void) cableEndPointNominated:(LCEntity *)iface
{
	/* Called when the endpoint for a cable is
	 * nominated by the user (i.e command-clicking
     * on an iface.
     */
	
	if ([self cableEndPointA])
	{
		/* A-End Nominated. Add cable! */
		LCVRackCable *cable = [[LCVRackCable alloc] initWithAEnd:[self cableEndPointA] bEnd:iface];
		[rack insertObject:cable inCablesAtIndex:0];
		[self setCableEndPointA:nil];
	}
	else
	{
		/* Set A-End for new cable */
		[self setCableEndPointA:iface];
	}
}

- (LCEntity *) cableEndPointA
{ return cableEndpointA; }

- (void) setCableEndPointA:(LCEntity *)newIface
{
	if (cableEndpointA) [cableEndpointA release];
	cableEndpointA = [newIface retain];
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (LCVRackCable *) hoverCable
{ return hoverCable; }

- (void) setHoverCable:(LCVRackCable *)newCable
{
	if (newCable != hoverCable) 
	{
		[hoverCable setHighlighted:NO];
		[hoverCable release];
		hoverCable = [newCable retain];
		[hoverCable setHighlighted:YES];
//		[cableView setPathsValid:NO];	
//		[cableView setNeedsDisplay:YES];
	}
}

- (LCVRackCable *) selectedCable
{ return selectedCable; }

- (void) setSelectedCable:(LCVRackCable *)newCable
{
	if (selectedCable) [selectedCable release];
	selectedCable = [newCable retain];
}

- (NSMutableArray *) cablesForInterface:(LCEntity *)entity
{
	NSMutableArray *matchList = [NSMutableArray array];
	NSEnumerator *cableEnum = [[rack cables] objectEnumerator];
	LCVRackCable *cable;
	while ((cable=[cableEnum nextObject])!=nil)
	{
		if ([cable aEndInterface] == entity || [cable bEndInterface] == entity)
		{ [matchList addObject:cable]; }
	}
	
	return matchList;
}

- (IBAction) highlightCablesClicked:(id)sender
{
	[self setCableHighlightDevice:hoverEntity];
}

- (LCEntity *) cableHighlightDevice
{
	return cableHighlightDevice;
}

- (void) setCableHighlightDevice:(LCEntity *)newEntity
{
	if (cableHighlightDevice) [cableHighlightDevice release];
	cableHighlightDevice = [newEntity retain];
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (IBAction) removeCableClicked:(id)sender
{
	if (hoverCable)
	{
		if ([[rack cables] containsObject:hoverCable])
		{ [rack removeObjectFromCablesAtIndex:[[rack cables] indexOfObject:hoverCable]]; }
	}
}

- (IBAction) setCableColourClicked:(id)sender
{
	/* Open colour sheet */
	[NSApp beginSheet:cableColourSheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) cableColourSheetCloseClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:cableColourSheet];
	[cableColourSheet close];
}

- (BOOL) showCableStatus
{ return showCableStatus; }

- (void) setShowCableStatus:(BOOL)flag
{ 
	showCableStatus = flag;
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (IBAction) cableAEndBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[selectedCable aEndInterface]] autorelease];
}

- (IBAction) cableAEndFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[selectedCable aEndInterface]];
}

- (IBAction) cableBEndBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[selectedCable bEndInterface]] autorelease];
}

- (IBAction) cableBEndFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[selectedCable bEndInterface]];
}

#pragma mark "Device Methods"

- (void) resetDeviceViews
{
	/* Clear old front views */
	for (LCVRackDeviceFrontView *devView in devFrontViews)
	{ [devView removeViewAndContent]; }
	[devFrontViews removeAllObjects];
	
	/* Clear out old back views */
	for (LCVRackDeviceBackView *devView in devBackViews)
	{ [devView removeViewAndContent]; }
	[devBackViews removeAllObjects];
	
	/* Add new views */
	NSEnumerator *devEnum = [[rack devices] objectEnumerator];
	LCVRackDevice *rDev;
	while ((rDev=[devEnum nextObject])!=nil)
	{
		/* Get rects */
		NSRect frontRect = [frontView rectForDevice:rDev];
		NSRect backRect = [backView rectForDevice:rDev];
		
		/* Create front view */
		LCVRackDeviceFrontView *frontDevView = [[[LCVRackDeviceFrontView alloc] initWithDevice:rDev inFrame:frontRect] autorelease];
		[frontDevView setRackView:backView];
		[frontDevView setRackController:self];
		[devFrontViews addObject:frontDevView];
		[frontView addSubview:frontDevView positioned:NSWindowBelow relativeTo:cableView];
		
		/* Create back view */
		LCVRackDeviceBackView *backDevView = [[[LCVRackDeviceBackView alloc] initWithDevice:rDev inFrame:backRect] autorelease];
		[backDevView setRackView:backView];
		[backDevView setRackController:self];
		[devBackViews addObject:backDevView];
		[backView addSubview:backDevView positioned:NSWindowBelow relativeTo:cableView];
	}	
}

- (id) deviceBackViewForEntity:(LCEntity *)entity
{
	LCVRackDeviceBackView *view;
	for (view in devBackViews)
	{
		if ([[view rackDevice] entity] == entity)
		{ return view; }
	}
	
	return nil;
}

- (IBAction) removeDeviceClicked:(id)sender
{
	[rack removeDevice:[self hoverDevice]];
}

- (IBAction) increaseSizeClicked:(id)sender
{
	[rack incrementSizeOfDevice:[self hoverDevice]];
}

- (IBAction) decreaseSizeClicked:(id)sender
{	
	[rack decrementSizeOfDevice:[self hoverDevice]];
}

- (LCVRackDevice *) hoverDevice
{ return hoverDevice; }

- (void) setHoverDevice:(LCVRackDevice *)newDevice
{
	if (hoverDevice) [hoverDevice release];
	hoverDevice = [newDevice retain];
}

- (LCVRackDevice *) selectedDevice
{ return selectedDevice; }

- (void) setSelectedDevice:(LCVRackDevice *)newDevice
{
	if (selectedDevice) [selectedDevice release];
	selectedDevice = [newDevice retain];
}

- (IBAction) setDeviceColourClicked:(id)sender
{
	/* Open device colour sheet */
	[NSApp beginSheet:deviceColourSheet
	   modalForWindow:window
		modalDelegate:self
	   didEndSelector:nil
		  contextInfo:nil];
}

- (IBAction) deviceColourSheetCloseClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:deviceColourSheet];
	[deviceColourSheet close];
}

- (IBAction) deviceBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[selectedDevice entity]] autorelease];
}

- (IBAction) deviceFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[selectedDevice entity]];
}

#pragma mark "Interface Methods"

- (IBAction) interfaceBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:hoverObject] autorelease];
}

- (IBAction) interfaceFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:hoverObject];
}

#pragma mark "Cable Group Methods"

- (IBAction) addCableGroupClicked:(id)sender
{
	LCVRackCableGroup *group = [[[LCVRackCableGroup alloc] init] autorelease];
	group.desc = @"New Group";
	[rack insertObject:group inCableGroupsAtIndex:rack.cableGroups.count];
}

- (IBAction) removeCableGroupClicked:(id)sender
{
	NSArray *selected = [cableGroupArrayController selectedObjects];
	LCVRackCableGroup *group;
	for (group in selected)
	{ [rack removeCableGroup:group]; }
}

#pragma mark "Cable List Methods"

- (IBAction) addNewCableClicked:(id)sender
{
	[rack insertObject:[[LCVRackCable new] autorelease] inCablesAtIndex:rack.cables.count];
}

- (IBAction) deleteSelectedCableClicked:(id)sender
{
	NSEnumerator *cableEnum = [[cableArrayController selectedObjects] objectEnumerator];
	LCVRackCable *cable;
	while (cable=[cableEnum nextObject])
	{ [rack removeObjectFromCablesAtIndex:[[rack cables] indexOfObject:cable]]; }
}

- (IBAction) lockAllCablesClicked:(id)sender
{
	NSEnumerator *cableEnum = [[rack cables] objectEnumerator];
	LCVRackCable *cable;
	while (cable=[cableEnum nextObject])
	{ [cable setLocked:YES]; }
}

- (IBAction) unlockAllCablesClicked:(id)sender
{
	NSEnumerator *cableEnum = [[rack cables] objectEnumerator];
	LCVRackCable *cable;
	while (cable=[cableEnum nextObject])
	{ [cable setLocked:NO]; }
}

- (NSString *) cableListSearchString
{ return cableListSearchString; }

- (void) setCableListSearchString:(NSString *)string
{ 
	if (cableListSearchString) 
	{
		[cableListSearchString release];
		cableListSearchString = nil;
	}
	
	if (string)
	{
		/* Filter string set */
		cableListSearchString = [string copy];
		
		/* Create filter predicate */
		NSPredicate *predicate = [NSPredicate predicateWithFormat:@"(aEndInterface.desc CONTAINS[cd] %@) OR (aEndDevice.name CONTAINS[cd] %@) OR (aEndDevice.desc CONTAINS[cd] %@) OR (bEndInterface.desc CONTAINS[cd] %@) OR (bEndDevice.name CONTAINS[cd] %@) OR (bEndDevice.desc CONTAINS[cd] %@) OR (vlans CONTAINS[cd] %@) OR (notes CONTAINS[cd] %@) OR (cableGroup.desc CONTAINS[cd] %@)",
			string, string, string, string, string, string, string, string, string, string];
		[self setCableListFilterPredicate:predicate];
	}
	else
	{
		[self setCableListFilterPredicate:nil];
	}
}

- (NSPredicate *) cableListFilterPredicate
{ return cableListFilterPredicate; }
- (void) setCableListFilterPredicate:(NSPredicate *)predicate
{ 
	[cableListFilterPredicate release];
	cableListFilterPredicate = [predicate retain];
}

#pragma mark Misc Accessor Methods

- (NSMenu *) cableMenu
{ return cableMenu; }
- (NSMenu *) deviceMenu
{ return deviceMenu; }
- (NSMenu *) interfaceMenu
{ return interfaceMenu; }
- (NSMenu *) rackUnitMenu
{ return rackUnitMenu; }
- (NSMutableArray *) devBackViews
{ return devBackViews; }
- (NSPoint) currentMouseLocationInWindow
{ return currentMouseLocationInWindow; }
- (void) setCurrentMouseLocationInWindow:(NSPoint)newPoint
{ 
	currentMouseLocationInWindow = newPoint; 
	if ([self cableEndPointA])
	{
		/* There is an endpoint nominated, 
		 * redraw the cable to keep the fake-cable
		 * updated.
		 */
		[cableView setNeedsDisplay:YES];
	}
}
- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }


@synthesize frontView;
@synthesize backView;
@synthesize devFrontViews;
@synthesize devBackViews;
@synthesize deviceColourSheet;
@synthesize cableView;
@synthesize cableColourSheet;
@synthesize cableArrayController;
@synthesize cableGroupArrayController;
@synthesize rackUnitMenu;
@synthesize deviceMenu;
@synthesize interfaceMenu;
@synthesize cableMenu;
@synthesize window;
@synthesize cableEndpointA;
@end
