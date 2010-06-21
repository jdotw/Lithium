//
//  LCVRackController.h
//  Lithium Console
//
//  Created by James Wilson on 12/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCVRackDocument.h"
#import "LCVRackView.h"
#import "LCCustomerList.h"

@interface LCVRackController : NSObject 
{
	/* Rack */
	LCVRackDocument *rack;


	
	
	
}

#pragma mark Shutdown
- (void) shutdown;

#pragma mark Rack Methods
@property (nonatomic,retain) LCVRackDocument *rack;

#pragma mark Hover Entity Methods
- (LCEntity *) hoverEntity;
- (void) setHoverEntity:(LCEntity *)newEntity;
- (int) hoverObjectType;
- (void) setHoverObjectType:(int)type;
- (id) hoverObject;
- (void) setHoverObject:(id)newObject;

#pragma mark Cable Methods
- (void) resetCableViews;
- (void) resizeCableViews;
- (LCVRackCableView *) cableView;
- (void) cableEndPointNominated:(LCEntity *)iface;
- (LCEntity *) cableEndPointA;
- (void) setCableEndPointA:(LCEntity *)newEntity;
- (LCVRackCable *) hoverCable;
- (void) setHoverCable:(LCVRackCable *)newCable;
- (LCVRackCable *) selectedCable;
- (void) setSelectedCable:(LCVRackCable *)newCable;
- (NSMutableArray *) cablesForInterface:(LCEntity *)entity;
- (IBAction) highlightCablesClicked:(id)sender;
- (LCEntity *) cableHighlightDevice;
- (void) setCableHighlightDevice:(LCEntity *)newEntity;
- (IBAction) removeCableClicked:(id)sender;
- (IBAction) setCableColourClicked:(id)sender;
- (IBAction) cableColourSheetCloseClicked:(id)sender;
- (BOOL) showCableStatus;
- (void) setShowCableStatus:(BOOL)flag;
- (IBAction) cableAEndBrowseClicked:(id)sender;
- (IBAction) cableAEndFaultHistoryClicked:(id)sender;
- (IBAction) cableBEndBrowseClicked:(id)sender;
- (IBAction) cableBEndFaultHistoryClicked:(id)sender;

#pragma mark Device Methods
- (void) resetDeviceViews;
- (id) deviceBackViewForEntity:(LCEntity *)entity;
- (IBAction) removeDeviceClicked:(id)sender;
- (IBAction) increaseSizeClicked:(id)sender;
- (IBAction) decreaseSizeClicked:(id)sender;
- (LCVRackDevice *) hoverDevice;
- (void) setHoverDevice:(LCVRackDevice *)newDevice;
- (LCVRackDevice *) selectedDevice;
- (void) setSelectedDevice:(LCVRackDevice *)newDevice;
- (IBAction) setDeviceColourClicked:(id)sender;
- (IBAction) deviceColourSheetCloseClicked:(id)sender;
- (IBAction) deviceBrowseClicked:(id)sender;
- (IBAction) deviceFaultHistoryClicked:(id)sender;

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
- (NSString *) cableListSearchString;
- (void) setCableListSearchString:(NSString *)string;
- (NSPredicate *) cableListFilterPredicate;
- (void) setCableListFilterPredicate:(NSPredicate *)predicate;

#pragma mark Accessor Methods
- (NSMenu *) rackUnitMenu;
- (NSMenu *) deviceMenu;
- (NSMenu *) interfaceMenu;
- (NSMenu *) cableMenu;
- (NSMutableArray *) devBackViews;
- (NSPoint) currentMouseLocationInWindow;
- (void) setCurrentMouseLocationInWindow:(NSPoint)newPoint;
- (LCCustomerList *) customerList;

@property (nonatomic,retain) LCVRackView *frontView;
@property (nonatomic,retain) LCVRackView *backView;
@property (nonatomic,retain) NSMutableArray *devFrontViews;
@property (retain,getter=devBackViews) NSMutableArray *devBackViews;
@property (nonatomic,retain) NSWindow *deviceColourSheet;
@property (retain,getter=cableView) LCVRackCableView *cableView;
@property (nonatomic,retain) NSWindow *cableColourSheet;
@property (getter=showCableStatus,setter=setShowCableStatus:) BOOL showCableStatus;
@property (nonatomic,retain) NSArrayController *cableArrayController;
@property (nonatomic,retain) NSArrayController *cableGroupArrayController;
@property (retain,getter=cableListFilterPredicate,setter=setCableListFilterPredicate:) NSPredicate *cableListFilterPredicate;
@property (retain,getter=rackUnitMenu) NSMenu *rackUnitMenu;
@property (retain,getter=deviceMenu) NSMenu *deviceMenu;
@property (retain,getter=interfaceMenu) NSMenu *interfaceMenu;
@property (retain,getter=cableMenu) NSMenu *cableMenu;
@property (nonatomic,retain) NSWindow *window;
@property (retain,getter=cableEndPointA) LCEntity *cableEndpointA;
@property (getter=hoverObjectType,setter=setHoverObjectType:) int hoverObjectType;
@end
