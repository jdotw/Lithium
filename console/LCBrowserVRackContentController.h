//
//  LCBrowserVRackContentController.h
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"
#import "LCDocument.h"
#import "LCDocumentBreadcrumView.h"
#import "LCBrowserDocumentContentController.h"
#import "LCVRackDocument.h"
#import "LCEntity.h"
#import "LCVRackView.h"
#import "LCInspectorController.h"
#import "LCInsetView.h"

@interface LCBrowserVRackContentController : LCBrowserDocumentContentController 
{
	/* Rack Document */
	LCVRackDocument *rack; 
	
	/* Rack Views */
	IBOutlet LCVRackView *frontView;
	IBOutlet LCVRackView *backView;
	IBOutlet LCInsetView *insetView;
	
	/* Devices */
	NSMutableArray *devFrontViews;
	NSMutableDictionary *devFrontViewDict;
	NSMutableArray *devBackViews;
	NSMutableDictionary *devBackViewDict;
	LCVRackDevice *colorChangeDevice;

	/* Cables */
	LCVRackCableView *cableView;
	LCEntity *cableHighlightDevice;
	BOOL showCableStatus;
	LCEntity *cableEndPointA;
	IBOutlet NSArrayController *cableArrayController;
	LCVRackCable *colorChangeCable;

	/* Color Selection */
	IBOutlet NSWindow *cableColourSheet;
	IBOutlet NSWindow *deviceColourSheet;
	
	/* Cable Groups */
	IBOutlet NSArrayController *cableGroupArrayController;

	/* Cable List */	
	NSString *cableListSearchString;
	NSPredicate *cableListFilterPredicate;
	
	/* Menus */
	IBOutlet NSMenu *rackUnitMenu;
	IBOutlet NSMenu *deviceMenu;
	IBOutlet NSMenu *interfaceMenu;
	IBOutlet NSMenu *cableMenu;
	
	/* Misc UI Elements */
	IBOutlet NSSplitView *splitView;
	IBOutlet LCInspectorController *inspectorController;

	/* Selection */
	LCEntity *selectedEntity;					/* Inspector binding */
	id selectedObject;							/* LCVRackDevice / LCVRackCable / LCVRackInterface */
	BOOL deviceSelected;
	BOOL interfaceSelected;
	BOOL cableSelected;
	
	/* Mouse */
	NSPoint currentMouseLocationInWindow;
}

#pragma mark "Constructors"
- (id) initWithDocument:(LCDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser;
@property (retain) LCVRackDocument *rack;

#pragma mark "Selection"
@property (retain) LCEntity *selectedEntity;
@property (retain) id selectedObject;
@property (assign) BOOL deviceSelected;
@property (assign) BOOL interfaceSelected;
@property (assign) BOOL cableSelected;

#pragma mark "Cable Methods"
- (void) resetCableViews;
- (void) resizeCableViews;
@property (readonly) LCVRackCableView *cableView;
- (void) cableEndPointNominated:(LCEntity *)iface;
@property (retain) LCEntity *cableEndPointA;
- (NSMutableArray *) cablesForInterface:(LCEntity *)entity;
- (IBAction) highlightCablesClicked:(id)sender;
@property (retain) LCEntity *cableHighlightDevice;
- (IBAction) removeCableClicked:(id)sender;
- (IBAction) setCableColourClicked:(id)sender;
- (IBAction) cableColourSheetCloseClicked:(id)sender;
@property (assign) BOOL showCableStatus;
- (IBAction) cableAEndBrowseClicked:(id)sender;
- (IBAction) cableAEndFaultHistoryClicked:(id)sender;
- (IBAction) cableBEndBrowseClicked:(id)sender;
- (IBAction) cableBEndFaultHistoryClicked:(id)sender;
@property (retain) LCVRackCable *colorChangeCable;

#pragma mark Device Methods
- (void) resetDeviceViews;
- (id) deviceBackViewForEntity:(LCEntity *)entity;
- (IBAction) removeDeviceClicked:(id)sender;
- (IBAction) increaseSizeClicked:(id)sender;
- (IBAction) decreaseSizeClicked:(id)sender;
- (IBAction) setDeviceColourClicked:(id)sender;
- (IBAction) deviceColourSheetCloseClicked:(id)sender;
- (IBAction) deviceBrowseClicked:(id)sender;
- (IBAction) deviceFaultHistoryClicked:(id)sender;
@property (retain) LCVRackDevice *colorChangeDevice;

#pragma mark "Interface Methods"
- (IBAction) interfaceBrowseClicked:(id)sender;
- (IBAction) interfaceFaultHistoryClicked:(id)sender;

#pragma mark Cable Group Methods
- (IBAction) addCableGroupClicked:(id)sender;
- (IBAction) removeCableGroupClicked:(id)sender;

#pragma mark "Cable List Methods"
- (IBAction) addNewCableClicked:(id)sender;
- (IBAction) deleteSelectedCableClicked:(id)sender;
- (IBAction) lockAllCablesClicked:(id)sender;
- (IBAction) unlockAllCablesClicked:(id)sender;
@property (copy) NSString *cableListSearchString;
@property (copy) NSPredicate *cableListFilterPredicate;

#pragma mark "Accessor Methods"
@property (readonly) NSMenu *rackUnitMenu;
@property (readonly) NSMenu *deviceMenu;
@property (readonly) NSMenu *interfaceMenu;
@property (readonly) NSMenu *cableMenu;
@property (readonly) NSMutableArray *devBackViews;
@property (readonly) NSMutableDictionary *devBackViewDict;
@property (assign) NSPoint currentMouseLocationInWindow;
@property (readonly) LCInsetView *insetView;
- (LCCustomerList *) customerList;

@end
