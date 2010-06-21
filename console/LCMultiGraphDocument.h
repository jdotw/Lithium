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
	
@property (nonatomic,retain) NSObjectController *controllerAlias;
@property (nonatomic,retain) LCMetricGraphEntityArrayController *graphSetMetricArrayController;
@property (nonatomic,retain) LCEntityArrayController *graphSetDeviceArrayController;
@property (nonatomic,retain) NSArrayController *metricSetArrayController;
@property (nonatomic,retain) NSWindow *configSheet;
@property (nonatomic,retain) NSWindow *graphSetSheet;
@property (getter=showDrawer,setter=setShowDrawer:) BOOL showDrawer;
@property (nonatomic,retain) NSButton *graphSetCreateButton;
@property (nonatomic,retain) NSObjectController *editGraphSetAlias;
@property (nonatomic,retain) NSObjectController *currentSetAlias;
@property (nonatomic,retain) LCBackgroundView *backView;
@property (nonatomic,retain) LCMetricGraphSet *newGraphSet;
@property (nonatomic,retain) LCMetricGraphSet *editGraphSet;
@property (nonatomic,retain) NSEnumerator *setEnumerator;
@property (nonatomic,retain) NSEnumerator *deviceEnumerator;
@property (nonatomic,retain) NSTimer *displayTimer;
@property BOOL isPaused;
@property (nonatomic,retain) LCMetricGraphController *graphAController;
@property (nonatomic,retain) LCMetricGraphController *graphBController;
@property (nonatomic,retain) LCMetricGraphController *graphCController;
@property (nonatomic,retain) LCMetricGraphController *graphDController;
@property (nonatomic,retain) NSObjectController *graphADeviceAlias;
@property (nonatomic,retain) NSObjectController *graphBDeviceAlias;
@property (nonatomic,retain) NSObjectController *graphCDeviceAlias;
@property (nonatomic,retain) NSObjectController *graphDDeviceAlias;
@property (nonatomic,retain) NSObjectController *graphASetAlias;
@property (nonatomic,retain) NSObjectController *graphBSetAlias;
@property (nonatomic,retain) NSObjectController *graphCSetAlias;
@property (nonatomic,retain) NSObjectController *graphDSetAlias;
@property (nonatomic,retain) NSToolbar *toolbar;
@property (nonatomic,retain) NSMutableDictionary *toolbarItems;
@property (nonatomic,retain) NSMutableArray *toolbarSelectableItems;
@property (nonatomic,retain) NSMutableArray *toolbarDefaultItems;
@end
