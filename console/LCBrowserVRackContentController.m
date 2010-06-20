//
//  LCBrowserVRackContentController.m
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserVRackContentController.h"
#import "LCDocumentBreadcrumItem.h"
#import "LCBrowser2Controller.h"
#import "LCFaultHistoryController.h"
#import "LCVRackDeviceFrontView.h"
#import "LCVRackDeviceBackView.h"
#import "NSGradient-Selection.h"
#import <Quartz/Quartz.h>

@implementation LCBrowserVRackContentController

#pragma mark "Constructors"

- (id) initWithDocument:(LCDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"VRackContent" document:initDocument inBrowser:initBrowser];
	if (!self) return nil;
	
	/* Create device views array */
	devFrontViews = [[NSMutableArray array] retain];
	devFrontViewDict = [[NSMutableDictionary dictionary] retain];
	devBackViews = [[NSMutableArray array] retain];
	devBackViewDict = [[NSMutableDictionary dictionary] retain];
	
	/* Create cable view */
	cableView = [[LCVRackCableView alloc] initWithFrame:[backView rectForRack]];	
	[backView addSubview:cableView];
	[cableView setHitView:backView];
	[cableView setRackController:self];		
	
	/* Setup Breadcrum View */
	LCDocumentBreadcrumItem *item = [LCDocumentBreadcrumItem new];
	item.title = @"Virtual Racks";
	[crumView insertObject:item inItemsAtIndex:0];
	[item autorelease];
	item = [LCDocumentBreadcrumItem new];
	item.title = self.document.desc;
	item.document = self.document;
	[crumView insertObject:item inItemsAtIndex:1];
	[item autorelease];
	
	/* Setup splitview */
	[splitView setPosition:[splitView maxPossiblePositionOfDividerAtIndex:0] ofDividerAtIndex:0];
	
	/* Bind Inspector */
	[inspectorController bind:@"target"
					 toObject:self
				  withKeyPath:@"selectedEntity"
					  options:nil];
	
	return self;
}

- (void) removedFromBrowserWindow
{
	[rack removeObserver:self forKeyPath:@"devices"];
	[rack removeObserver:self forKeyPath:@"cables"];	
	[inspectorController unbind:@"target"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[rack release];
	[cableListSearchString release];
	[cableListFilterPredicate release];
	[devFrontViews release];
	[devFrontViewDict release];
	[devBackViews release];
	[devBackViewDict release];
	[colorChangeCable release];
	[colorChangeDevice release];
	[super dealloc];
}

- (IBAction) breadcrumClicked:(id)sender
{
	NSButton *button = sender;
	LCDocumentBreadcrumItem *crumItem = [[button cell] representedObject];
	if (!crumItem.document)
	{
		/* Select root */
		[browser.browserTreeOutlineView selectVRacksRoot];
	}
}

#pragma mark "XML Methods"

- (void) XMLRequestFinished:(LCXMLRequest *)sender
{
	[super XMLRequestFinished:sender];
	for (LCVRackDevice *rDev in rack.devices)
	{
		if (!rDev.entity.initialRefreshPerformed) 
		{ [rDev.entity highPriorityRefresh]; }
	}
	
}

- (void) commitDocument
{
	/* Update thumbnail */
//	for (LCVRackDeviceFrontView *devView in devFrontViews)
//	{ 
//		devView.drawThumbnail = YES; 
//		[devView setNeedsDisplay:YES];
//	}
//	[frontView setNeedsDisplay:YES];
//	[frontView lockFocus];
//	NSData *imageData = [frontView dataWithPDFInsideRect:[frontView bounds]];
//	[frontView unlockFocus];
//	LCVRackDocument *vrackDoc = (LCVRackDocument *) document;
//	vrackDoc.thumbnail = [[[NSImage alloc] initWithData:imageData] autorelease];
//	for (LCVRackDeviceFrontView *devView in devFrontViews)
//	{ devView.drawThumbnail = NO; }
	
	LCVRackDocument *vrackDoc = (LCVRackDocument *) document;
	NSBitmapImageRep *bitmapRep = [frontView bitmapImageRepForCachingDisplayInRect:[frontView bounds]];
	[frontView cacheDisplayInRect:[frontView bounds] toBitmapImageRep:bitmapRep];
	vrackDoc.thumbnail = [[[NSImage alloc] initWithData:[bitmapRep TIFFRepresentation]] autorelease];	
	
	/* Commit */
	[super commitDocument];
}

#pragma mark "Document Properties"

- (void) setDocument:(LCDocument *)value
{
	[super setDocument:value];
	self.rack = (LCVRackDocument *)document;
	
	/* Reset views */
	[self resetDeviceViews];
	[self resetCableViews];
}

@synthesize rack;
- (void) setRack:(LCVRackDocument *)value
{
	[rack removeObserver:self forKeyPath:@"devices"];
	[rack removeObserver:self forKeyPath:@"cables"];
	[rack release];
	rack = [value retain];
	
	[rack addObserver:self
		   forKeyPath:@"devices"
			  options:NSKeyValueObservingOptionOld|NSKeyValueObservingOptionNew
			  context:nil];
	[rack addObserver:self
		   forKeyPath:@"cables"
			  options:NSKeyValueObservingOptionOld|NSKeyValueObservingOptionNew
			  context:nil];
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
}

#pragma mark "Selection"
@synthesize selectedEntity;
@synthesize selectedObject;
- (void) setSelectedObject:(id)value
{
	/* Clear old selection */
	if ([selectedObject class] == [LCVRackDevice class])
	{
		LCVRackDevice *dev = (LCVRackDevice *)selectedObject;
		LCVRackDeviceFrontView *devFrontView = [devFrontViewDict objectForKey:[dev.entity.entityAddress addressString]];
		devFrontView.selected = NO;
		LCVRackDeviceBackView *devBackView = [devBackViewDict objectForKey:[dev.entity.entityAddress addressString]];
		devBackView.selected = NO;
		self.cableHighlightDevice = nil;
	}
	else if ([selectedObject class] == [LCVRackInterface class])
	{
		LCVRackInterface *iface = (LCVRackInterface *)selectedObject;
		LCVRackDeviceBackView *devBackView = [devBackViewDict objectForKey:[[iface.entity.device entityAddress] addressString]];
		devBackView.selectedInterfaceEntity = nil;
		devBackView.selected = NO;
	}
	else
	{

	}

	/* Set new selection */
	[selectedObject release];
	selectedObject = [value retain];	
	if ([selectedObject class] == [LCVRackDevice class])
	{
		LCVRackDevice *dev = (LCVRackDevice *)selectedObject;
		self.selectedEntity = dev.entity;
		self.deviceSelected = YES;
		self.interfaceSelected = NO;
		self.cableSelected = NO;
		LCVRackDeviceFrontView *devFrontView = [devFrontViewDict objectForKey:[dev.entity.entityAddress addressString]];
		devFrontView.selected = YES;
		LCVRackDeviceBackView *devBackView = [devBackViewDict objectForKey:[dev.entity.entityAddress addressString]];
		devBackView.selected = YES;		
		self.cableHighlightDevice = dev.entity;
	}
	else if ([selectedObject class] == [LCVRackInterface class])
	{
		LCVRackInterface *iface = (LCVRackInterface *)selectedObject;
		self.selectedEntity = iface.entity;
		self.deviceSelected = YES;
		self.interfaceSelected = YES;
		self.cableSelected = NO;
		LCVRackDeviceBackView *devBackView = [devBackViewDict objectForKey:[[iface.entity.device entityAddress] addressString]];
		devBackView.selectedInterfaceEntity = iface.entity;	
		devBackView.selected = YES;		
	}
	else
	{
		self.selectedEntity = nil;
		self.deviceSelected = NO;
		self.interfaceSelected = NO;
		self.cableSelected = YES;
	}
}

@synthesize deviceSelected;
@synthesize interfaceSelected;
@synthesize cableSelected;

#pragma mark "UI Validation"

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item
{
	SEL action = [item action];
	
	/*
	 * Cable Actions
	 */
	
	if (action == @selector(highlightCablesClicked:) && selectedObject) return YES;
	if (action == @selector(removeCableClicked:) && cableSelected && rack.editing) return YES;
	if (action == @selector(setCableColourClicked:) && cableSelected && rack.editing) return YES;
	if (action == @selector(cableAEndBrowseClicked:) && cableSelected) return YES;
	if (action == @selector(cableAEndFaultHistoryClicked:) && cableSelected) return YES;
	if (action == @selector(cableBEndBrowseClicked:) && cableSelected) return YES;
	if (action == @selector(cableBEndFaultHistoryClicked:) && cableSelected) return YES;
	
	/*
	 * Device
	 */
	
	if (action == @selector(removeDeviceClicked:) && deviceSelected && rack.editing) return YES; 
	if (action == @selector(increaseSizeClicked:) && deviceSelected && rack.editing) return YES; 
	if (action == @selector(decreaseSizeClicked:) && deviceSelected && rack.editing) return YES; 
	if (action == @selector(setDeviceColourClicked:) && deviceSelected && rack.editing) return YES; 
	if (action == @selector(deviceBrowseClicked:) && deviceSelected) return YES; 
	if (action == @selector(deviceFaultHistoryClicked:) && deviceSelected) return YES; 
	
	/*
	 * Interface 
	 */
	
	if (action == @selector(interfaceBrowseClicked:) && interfaceSelected) return YES; 
	if (action == @selector(interfaceFaultHistoryClicked:) && interfaceSelected) return YES; 

	
	/*
	 * Cable Groups 
	 */
	
	if (action == @selector(addCableGroupClicked:) && rack.editing) return YES;
	if (action == @selector(removeCableGroupClicked:) && rack.editing) return YES;
	
	/*
	 * Cable 
	 */
	
	if (action == @selector(addNewCableClicked:) && rack.editing) return YES;
	if (action == @selector(deleteSelectedCableClicked:) && cableSelected && rack.editing) return YES;
	
	/* Default */
	return NO;
}	


#pragma mark "Cable Methods"

@synthesize cableView;

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
		[(LCVRackDocument *)document insertObject:cable inCablesAtIndex:0];
		[cable autorelease];
		[self setCableEndPointA:nil];
	}
	else
	{
		/* Set A-End for new cable */
		[self setCableEndPointA:iface];
	}
}

@synthesize cableEndPointA;
- (void) setCableEndPointA:(LCEntity *)newIface
{
	if (cableEndPointA) [cableEndPointA release];
	cableEndPointA = [newIface retain];
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (NSMutableArray *) cablesForInterface:(LCEntity *)entity
{
	NSMutableArray *matchList = [NSMutableArray array];
	NSEnumerator *cableEnum = [[self.rack cables] objectEnumerator];
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
	[self setCableHighlightDevice:selectedEntity];
}

@synthesize cableHighlightDevice;
- (void) setCableHighlightDevice:(LCEntity *)newEntity
{
	if (cableHighlightDevice) [cableHighlightDevice release];
	cableHighlightDevice = [newEntity retain];
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (IBAction) removeCableClicked:(id)sender
{
	if ([selectedObject class] == [LCVRackCable class])
	{
		if ([[self.rack cables] containsObject:selectedObject])
		{ [self.rack removeObjectFromCablesAtIndex:[[self.rack cables] indexOfObject:selectedObject]]; }
	}
}

- (IBAction) setCableColourClicked:(id)sender
{
	/* Open colour sheet */
	if ([selectedObject class] == [LCVRackCable class])
	{
		self.colorChangeCable = selectedObject;
	
		[NSApp beginSheet:cableColourSheet
		   modalForWindow:[[self view] window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

- (IBAction) cableColourSheetCloseClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:cableColourSheet];
	[cableColourSheet close];
}

@synthesize showCableStatus;
- (void) setShowCableStatus:(BOOL)flag
{ 
	showCableStatus = flag;
	[cableView setPathsValid:NO];
	[cableView setNeedsDisplay:YES];
}

- (IBAction) cableAEndBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[selectedObject aEndInterface]] autorelease];
}

- (IBAction) cableAEndFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[selectedObject aEndInterface]];
}

- (IBAction) cableBEndBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:[selectedObject bEndInterface]] autorelease];
}

- (IBAction) cableBEndFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:[selectedObject bEndInterface]];
}

@synthesize colorChangeCable;

#pragma mark "Device Methods"

- (void) resetDeviceViews
{
	/* Add new views */
	for (LCVRackDevice *rDev in rack.devices)
	{
		/* Get rects */
		NSRect frontRect = [frontView rectForDevice:rDev];
		NSRect backRect = [backView rectForDevice:rDev];
		
		/* Create front view */
		LCVRackDeviceFrontView *frontDevView = [devFrontViewDict objectForKey:[[rDev.entity entityAddress] addressString]];
		if (frontDevView)
		{
			[frontDevView setFrame:frontRect];
		}
		else
		{
			frontDevView = [[[LCVRackDeviceFrontView alloc] initWithDevice:rDev inFrame:frontRect] autorelease];
			[frontDevView setRackView:backView];
			[frontDevView setRackController:self];
			[devFrontViews addObject:frontDevView];
			[devFrontViewDict setObject:frontDevView forKey:[[rDev.entity entityAddress] addressString]];
			[frontView addSubview:frontDevView positioned:NSWindowBelow relativeTo:cableView];
		}
		
		/* Create back view */
		LCVRackDeviceBackView *backDevView = [devBackViewDict objectForKey:[[rDev.entity entityAddress] addressString]];
		if (backDevView)
		{
			[backDevView setFrame:backRect];
		}
		else 
		{
			backDevView = [[[LCVRackDeviceBackView alloc] initWithDevice:rDev inFrame:backRect] autorelease];
			[backDevView setRackView:backView];
			[backDevView setRackController:self];
			[devBackViews addObject:backDevView];
			[devBackViewDict setObject:backDevView forKey:[[rDev.entity entityAddress] addressString]];
			[backView addSubview:backDevView positioned:NSWindowBelow relativeTo:cableView];
		}
	}	

	/* Clear obsolete front views */
	NSMutableArray *removeArray = [NSMutableArray array];
	for (LCVRackDeviceFrontView *devView in devFrontViews)
	{ 
		if (![rack.devices containsObject:devView.rackDevice])
		{ [removeArray addObject:devView]; }
	}
	for (LCVRackDeviceFrontView *devView in removeArray)
	{
		[devView removeViewAndContent];
		[devFrontViews removeObject:devView];
		[devFrontViewDict removeObjectForKey:[[devView.rackDevice.entity entityAddress] addressString]];
	}
	[removeArray removeAllObjects];
	
	/* Clear out old back views */
	for (LCVRackDeviceBackView *devView in devBackViews)
	{
		if (![rack.devices containsObject:devView.rackDevice])
		{ [removeArray addObject:devView]; }
	}
	for (LCVRackDeviceBackView *devView in removeArray)
	{ 
		[devView removeViewAndContent]; 
		[devBackViews removeObject:devView];
		[devBackViewDict removeObjectForKey:[[devView.rackDevice.entity entityAddress] addressString]];
	}	
	[removeArray removeAllObjects];
}

- (id) deviceBackViewForEntity:(LCEntity *)entity
{
	return [devBackViewDict objectForKey:[[entity entityAddress] addressString]];
}

- (IBAction) removeDeviceClicked:(id)sender
{
	if ([selectedObject class] == [LCVRackDevice class])
	{ [self.rack removeDevice:[self selectedObject]]; }
}

- (IBAction) increaseSizeClicked:(id)sender
{
	if ([selectedObject class] == [LCVRackDevice class])
	{ [self.rack incrementSizeOfDevice:[self selectedObject]]; }
	[self resetDeviceViews];
	[self resetCableViews];
}

- (IBAction) decreaseSizeClicked:(id)sender
{	
	if ([selectedObject class] == [LCVRackDevice class])
	{ [self.rack decrementSizeOfDevice:[self selectedObject]]; }
	[self resetDeviceViews];
	[self resetCableViews];
}

- (IBAction) setDeviceColourClicked:(id)sender
{
	/* Open device colour sheet */
	if ([selectedObject class] == [LCVRackDevice class])
	{
		self.colorChangeDevice = selectedObject;
		[NSApp beginSheet:deviceColourSheet
		   modalForWindow:[[self view] window]
			modalDelegate:self
		   didEndSelector:nil
			  contextInfo:nil];
	}
}

- (IBAction) deviceColourSheetCloseClicked:(id)sender
{
	/* Close sheet */
	[NSApp endSheet:deviceColourSheet];
	[deviceColourSheet close];
}

- (IBAction) deviceBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:(LCEntity *)selectedEntity.device] autorelease];
}

- (IBAction) deviceFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:selectedEntity.device];
}

@synthesize colorChangeDevice;

#pragma mark "Interface Methods"

- (IBAction) interfaceBrowseClicked:(id)sender
{
	[[[LCBrowser2Controller alloc] initWithEntity:selectedEntity] autorelease];
}

- (IBAction) interfaceFaultHistoryClicked:(id)sender
{
	[[LCFaultHistoryController alloc] initForEntity:selectedEntity];
}

#pragma mark "Cable Group Methods"

- (IBAction) addCableGroupClicked:(id)sender
{
	LCVRackCableGroup *group = [[[LCVRackCableGroup alloc] init] autorelease];
	group.desc = @"New Group";
	[self.rack insertObject:group inCableGroupsAtIndex:self.rack.cableGroups.count];
}

- (IBAction) removeCableGroupClicked:(id)sender
{
	NSArray *selected = [cableGroupArrayController selectedObjects];
	LCVRackCableGroup *group;
	for (group in selected)
	{ [self.rack removeCableGroup:group]; }
}

#pragma mark "Cable List Methods"

- (IBAction) addNewCableClicked:(id)sender
{
	[self.rack insertObject:[[LCVRackCable new] autorelease] inCablesAtIndex:self.rack.cables.count];
}

- (IBAction) deleteSelectedCableClicked:(id)sender
{
	NSEnumerator *cableEnum = [[cableArrayController selectedObjects] objectEnumerator];
	LCVRackCable *cable;
	while (cable=[cableEnum nextObject])
	{ [self.rack removeObjectFromCablesAtIndex:[[self.rack cables] indexOfObject:cable]]; }
}

- (IBAction) lockAllCablesClicked:(id)sender
{
	NSEnumerator *cableEnum = [[self.rack cables] objectEnumerator];
	LCVRackCable *cable;
	while (cable=[cableEnum nextObject])
	{ [cable setLocked:YES]; }
}

- (IBAction) unlockAllCablesClicked:(id)sender
{
	NSEnumerator *cableEnum = [[self.rack cables] objectEnumerator];
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

@synthesize devBackViews;
@synthesize devBackViewDict;
- (NSMenu *) cableMenu
{ return cableMenu; }
- (NSMenu *) deviceMenu
{ return deviceMenu; }
- (NSMenu *) interfaceMenu
{ return interfaceMenu; }
- (NSMenu *) rackUnitMenu
{ return rackUnitMenu; }
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

@synthesize insetView;

@end
