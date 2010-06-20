//
//  LCFaultHistoryController.h
//  Lithium Console
//
//  Created by James Wilson on 30/11/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCCaseList.h"
#import "LCIncidentList.h"
#import "LCBackgroundView.h"

@interface LCFaultHistoryController : NSWindowController 
{
	/* Entity */
	LCEntity *entity;
	
	/* NIB Elements */
	IBOutlet NSObjectController *controllerAlias;
	IBOutlet LCBackgroundView *backView;
	
	/* Cases */
	LCCaseList *caseList;
	
	/* Incident */
	LCIncidentList *incidentList;

	/* Sort descriptor */
	NSArray *caseArraySortDescriptors;
	NSArray *incidentArraySortDescriptors;

	/* Toolbar */
	NSToolbar *toolbar;
	NSMutableDictionary *toolbarItems;
	NSMutableArray *toolbarSelectableItems;
	NSMutableArray *toolbarDefaultItems;	
}

#pragma mark Initialisation
- (LCFaultHistoryController *) initForEntity:(LCEntity *)initEntity;
- (void) dealloc;

#pragma mark "Toolbar Methods"
- (void) buildToolbar;
- (NSToolbarItem*) toolbar: (NSToolbar*) theToolbar
	 itemForItemIdentifier: (NSString*) identifier 
 willBeInsertedIntoToolbar: (BOOL) flag;
- (NSArray*) toolbarAllowedItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarDefaultItemIdentifiers: (NSToolbar*) theToolbar;
- (NSArray*) toolbarSelectableItemIdentifiers: (NSToolbar*) theToolbar;
- (IBAction) toggleToolbarClicked:(id)sender;

#pragma mark "User Interface Action Methods"
- (IBAction) incidentTableViewDoubleClicked:(NSArray *)selectedObjects;
- (IBAction) caseTableViewDoubleClicked:(NSArray *)selectedObjects;
- (IBAction) refreshIncidentsClicked:(id)sender;
- (IBAction) refreshCasesClicked:(id)sender;

#pragma mark Accessor Methods
- (LCEntity *) entity;
- (void) setEntity:(LCEntity *)otherEntity;
- (LCCaseList *) caseList;
- (LCIncidentList *) incidentList;
- (NSArray *) incidentArraySortDescriptors;
- (void) setIncidentArraySortDescriptors:(NSArray *)array;
- (NSArray *) caseArraySortDescriptors;
- (void) setCaseArraySortDescriptors:(NSArray *)array;

@property (retain,getter=entity,setter=setEntity:) LCEntity *entity;
@property (retain) NSObjectController *controllerAlias;
@property (retain) LCBackgroundView *backView;
@property (retain,getter=caseList) LCCaseList *caseList;
@property (retain,getter=incidentList) LCIncidentList *incidentList;
@property (retain) NSToolbar *toolbar;
@property (retain) NSMutableDictionary *toolbarItems;
@property (retain) NSMutableArray *toolbarSelectableItems;
@property (retain) NSMutableArray *toolbarDefaultItems;
@end
