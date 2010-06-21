//
//  LCBWRepDocument.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBWRepItem.h"
#import "LCBWRepGroup.h"
#import "LCBWRepDevice.h"
#import "LCBWRepInterface.h"
#import "LCMetricHistory.h"

@interface LCBWRepDocument : NSDocument 
{
	NSMutableDictionary *properties;
	NSMutableDictionary *deviceItems;	/* Dictionary of local-level device items, keyed by entity address */
	NSMutableDictionary *interfaceItems;	/* Dictionary of interface items, keyed by entity address */
	NSMutableDictionary *groupItems;	/* Dictionary of group items, keyed by description */
	NSMutableArray *interfaceList;		/* List of interfaces */
	
	NSMutableArray *historyRefreshList;	/* List of metric history objects being refreshed */
	int totalMetricsBeingRefreshed;
	int metricsLeftToBeRefreshed;
	
	NSDate *referenceDate;				/* Reference date */
	NSTimer *referenceDateChangeTimer;
	
	id windowController;
}

#pragma mark "Constructors"
- (LCBWRepDocument *) init;
- (void) dealloc;

#pragma mark "NSDocument Methods"
- (NSString *)windowNibName;
- (void)makeWindowControllers;
- (NSData *)dataRepresentationOfType:(NSString *)type;
- (BOOL)loadDataRepresentation:(NSData *)data ofType:(NSString *)type;

#pragma mark "KVO Observing"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context;

#pragma mark "Device Methods"
- (NSMutableDictionary *) deviceItems;
- (LCBWRepDevice *) locateDeviceItem:(LCEntity *)device;
- (void) addDeviceItem:(LCBWRepDevice *)item;
- (void) removeDeviceItem:(LCBWRepDevice *)item;

#pragma mark "Group Methods"
- (NSMutableDictionary *) groupItems;
- (LCBWRepGroup *) locateGroupItem:(NSString *)description;
- (void) addGroupItem:(LCBWRepGroup *)item;
- (void) removeGroupItem:(LCBWRepGroup *)item;
- (void) updateGroup:(LCBWRepGroup *)item description:(NSString *)newDescription;

#pragma mark "Interface Methods"
- (NSMutableDictionary *) interfaceItems;
- (LCBWRepInterface *) locateInterfaceItem:(LCEntity *)device;
- (void) addInterfaceItem:(LCBWRepInterface *)item;
- (void) removeInterfaceItem:(LCBWRepInterface *)item;
- (NSMutableArray *) interfaceList;
- (unsigned int) countOfInterfaceList;
- (LCBWRepItem *) objectInInterfaceListAtIndex:(unsigned int)index;
- (void) insertObject:(LCBWRepInterface *)item inInterfaceListAtIndex:(unsigned int)index;
- (void) removeObjectFromInterfaceListAtIndex:(unsigned int)index;

#pragma mark "Refresh"
- (void) refresh:(int)priority;
- (NSMutableArray *) historyRefreshList;
- (unsigned int) countOfHistoryRefreshList;
- (LCBWRepItem *) objectInHistoryRefreshListAtIndex:(unsigned int)index;
- (void) insertObject:(LCMetricHistory *)item inHistoryRefreshListAtIndex:(unsigned int)index;
- (void) removeObjectFromHistoryRefreshListAtIndex:(unsigned int)index;
- (int) totalMetricsBeingRefreshed;
- (void) setTotalMetricsBeingRefreshed:(int)value;
- (int) metricsLeftToBeRefreshed;
- (void) setMetricsLeftToBeRefreshed:(int)value;
- (void) cancelRefresh;

#pragma mark "Export to CSV"
- (void) exportToCsv:(NSString *)filename;

#pragma mark "Accessors"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)newProperties;
- (int) statsMode;
- (void) setStatsMode:(int)value;
- (int) viewMode;
- (void) setViewMode:(int)value;
- (BOOL) discardMissing;
- (void) setDiscardMissing:(BOOL)value;
- (NSDate *) referenceDate;
- (void) setReferenceDate:(NSDate *)value;
- (int) referencePeriod;
- (void) setReferencePeriod:(int)value;

#pragma mark "Item Accessors"
- (NSMutableArray *) items;
- (void) setItems:(NSMutableArray *)array;
- (unsigned int) countOfItems;
- (LCBWRepItem *) objectInItemsAtIndex:(unsigned int)index;
- (void) insertObject:(LCBWRepItem *)item inItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromItemsAtIndex:(unsigned int)index;


@property (retain,getter=groupItems) NSMutableDictionary *groupItems;
@property (retain,getter=interfaceList) NSMutableArray *interfaceList;
@property (retain,getter=historyRefreshList) NSMutableArray *historyRefreshList;
@property (retain,getter=referenceDate,setter=setReferenceDate:) NSDate *referenceDate;
@property (nonatomic,retain) NSTimer *referenceDateChangeTimer;
@property (nonatomic,retain) id windowController;
@end
