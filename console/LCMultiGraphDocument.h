//
//  LCMultiGraphDocument.h
//  Lithium Console
//
//  Created by James Wilson on 9/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCMetricGraphSet.h"
#import "LCMetricGraphController.h"
#import "LCMetricGraphEntityArrayController.h"
#import "LCBackgroundView.h"

@interface LCMultiGraphDocument : NSDocument 
{
	/* NIB Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCMetricGraphEntityArrayController *graphSetMetricArrayController;
	IBOutlet LCEntityArrayController *graphSetDeviceArrayController;
	IBOutlet NSArrayController *metricSetArrayController;
	IBOutlet NSWindow *configSheet;
	IBOutlet NSWindow *graphSetSheet;
	BOOL showDrawer;
	IBOutlet NSButton *graphSetCreateButton;
	IBOutlet NSObjectController *editGraphSetAlias;
	IBOutlet NSObjectController *currentSetAlias;
	IBOutlet LCBackgroundView *backView;

	/* Properties etc */
	NSMutableDictionary *properties;
	LCMetricGraphSet *newGraphSet;
	LCMetricGraphSet *editGraphSet;

	/* Graph Display Variables */
	NSEnumerator *setEnumerator;
	NSEnumerator *deviceEnumerator;
	NSTimer *displayTimer;
	LCMetricGraphSet *currentMetricSet;
	BOOL isPaused;

	/* Graph Controllers */
	IBOutlet LCMetricGraphController *graphAController;
	IBOutlet LCMetricGraphController *graphBController;
	IBOutlet LCMetricGraphController *graphCController;
 	IBOutlet LCMetricGraphController *graphDController;
	
	/* Device objectControllers */
	IBOutlet NSObjectController *graphADeviceAlias;
	IBOutlet NSObjectController *graphBDeviceAlias;
	IBOutlet NSObjectController *graphCDeviceAlias;
	IBOutlet NSObjectController *graphDDeviceAlias;
	
	/* MetricSet objectControllers */
	IBOutlet NSObjectController *graphASetAlias;
	IBOutlet NSObjectController *graphBSetAlias;
	IBOutlet NSObjectController *graphCSetAlias;
	IBOutlet NSObjectController *graphDSetAlias;

	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
}

#pragma mark "Initialisation"
- (id)init;
- (void) dealloc;

#pragma mark "NSDocument Methods"
- (NSString *)windowNibName;
- (void)windowControllerDidLoadNib:(NSWindowController *) aController;
- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError;
- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError;

#pragma mark "Graph Display Methods"
- (void) resetDisplay;
- (void) displayTimerFired;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;

#pragma mark "Toolbar Actions"
- (void) toggleDrawerClicked:(id)sender;

#pragma mark "Graph Config Methods"
- (IBAction) graphConfigClicked:(id)sender;
- (IBAction) graphConfigCloseClicked:(id)sender;

#pragma mark "Graph Set Methods"
- (IBAction) graphSetAddClicked:(id)sender;
- (IBAction) graphSetRemoveClicked:(id)sender;
- (IBAction) graphSetCreateOrUpdateClicked:(id)sender;
- (IBAction) graphSetCancelClicked:(id)sender;

#pragma mark "Controller syncrhonisation Methods"
- (void) updateControllers;

#pragma mark "Accessor Methods"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;

- (NSMutableArray *) metricSets;
- (void) setMetricSets:(NSMutableArray *)array;
- (void) insertObject:(LCMetricGraphSet *)set inMetricSetsAtIndex:(unsigned int)index;
- (void) removeObjectFromMetricSetsAtIndex:(unsigned int)index;

- (LCMetricGraphSet *) currentMetricSet;
- (void) setCurrentMetricSet:(LCMetricGraphSet *)set;

- (BOOL) showDrawer;
- (void) setShowDrawer:(BOOL)flag;

- (NSNumber *) displayInterval;
- (void) setDisplayInterval:(NSNumber *)number;

- (int) graphPeriod;
- (void) setGraphPeriod:(int)period;

- (BOOL) useSpecificReferenceDate;
- (void) setUseSpecificReferenceDate:(BOOL)flag;

- (NSDate *) referenceDate;
- (void) setReferenceDate:(NSDate *)date;
	
@property (retain) NSObjectController *controllerAlias;
@property (retain) LCMetricGraphEntityArrayController *graphSetMetricArrayController;
@property (retain) LCEntityArrayController *graphSetDeviceArrayController;
@property (retain) NSArrayController *metricSetArrayController;
@property (retain) NSWindow *configSheet;
@property (retain) NSWindow *graphSetSheet;
@property (getter=showDrawer,setter=setShowDrawer:) BOOL showDrawer;
@property (retain) NSButton *graphSetCreateButton;
@property (retain) NSObjectController *editGraphSetAlias;
@property (retain) NSObjectController *currentSetAlias;
@property (retain) LCBackgroundView *backView;
@property (retain) LCMetricGraphSet *newGraphSet;
@property (retain) LCMetricGraphSet *editGraphSet;
@property (retain) NSEnumerator *setEnumerator;
@property (retain) NSEnumerator *deviceEnumerator;
@property (retain) NSTimer *displayTimer;
@property BOOL isPaused;
@property (retain) LCMetricGraphController *graphAController;
@property (retain) LCMetricGraphController *graphBController;
@property (retain) LCMetricGraphController *graphCController;
@property (retain) LCMetricGraphController *graphDController;
@property (retain) NSObjectController *graphADeviceAlias;
@property (retain) NSObjectController *graphBDeviceAlias;
@property (retain) NSObjectController *graphCDeviceAlias;
@property (retain) NSObjectController *graphDDeviceAlias;
@property (retain) NSObjectController *graphASetAlias;
@property (retain) NSObjectController *graphBSetAlias;
@property (retain) NSObjectController *graphCSetAlias;
@property (retain) NSObjectController *graphDSetAlias;
@property (retain) NSToolbar *toolbar;
@property (retain) NSMutableDictionary *toolbarItems;
@property (retain) NSMutableArray *toolbarSelectableItems;
@property (retain) NSMutableArray *toolbarDefaultItems;
@end
