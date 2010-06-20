//
//  LCIncident.h
//  Lithium Console
//
//  Created by James Wilson on 28/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"
#import "LCEntityDescriptor.h"
#import "LCXMLObject.h"

@interface LCIncident : LCXMLObject 
{
	/* Incident Properties */
	unsigned long incidentID;
	unsigned long caseID;
	int state;
	int type;
	BOOL isActive;
	BOOL isLive;
	BOOL hasCase;
	unsigned long startTimestamp;
	NSDate *startDate;
	NSString *startDateShortString;
	unsigned long endTimestamp;
	NSDate *endDate;
	NSString *endDateShortString;
	NSTimeInterval openTimeInterval;
	NSString *raisedValueString;
	NSString *clearedValueString;

	/* Entity */
	LCEntity *entity;
	LCEntityDescriptor *entityDescriptor;
	
	/* Actions */
	NSMutableArray *actions;
	NSMutableDictionary *actionDictionary;
	BOOL hasActions;
	BOOL hasPendingActions;			/* Actions about to run/re-run */

	/* Related Incident Properties */
	int relevanceScore;
	BOOL relatedSelected;
	BOOL relatedSelectionDisabled;
}

#pragma mark "Initialisation"
- (LCIncident *) init;
- (void) dealloc;

#pragma mark "Clearing Method"
- (void) clearIncident;

#pragma mark "Cancel Pending Actions"
- (void) cancelPendingActionsForIncidents:(NSArray *)incidents;

#pragma mark "Actions"
@property (readonly) NSMutableArray *actions;
@property (readonly) NSMutableDictionary *actionDictionary;
@property BOOL hasActions;
@property BOOL hasPendingActions;
- (void) insertObject:(id)action inActionsAtIndex:(unsigned int)index;
- (void) removeObjectFromActionsAtIndex:(unsigned int)index;
- (NSNumber *) actionCount;
- (NSImage *) actionIcon;

#pragma mark "Property Methods"
@property (assign) unsigned long incidentID;
@property (assign) unsigned long caseID;
@property (assign) int state;
@property (assign) int type;
@property (assign) BOOL isActive;
@property (assign) BOOL isLive;
@property (assign) BOOL hasCase;
@property (assign) unsigned long startTimestamp;
@property (copy) NSDate *startDate;
@property (copy) NSString *startDateShortString;
@property (assign) unsigned long endTimestamp;
@property (copy) NSDate *endDate;
@property (copy) NSString *endDateShortString;
@property (assign) NSTimeInterval openTimeInterval;
- (void) updateOpenTimeInterval;
@property (copy) NSString *raisedValueString;
@property (copy) NSString *clearedValueString;
@property (retain) LCEntityDescriptor *entityDescriptor;
@property (retain) LCEntity *entity;
@property (assign) int relevanceScore;
@property (assign) BOOL relatedSelected;
@property (assign) BOOL relatedSelectionDisabled;

#pragma mark "General Accessors"
- (NSArray *) incidents;
- (id) viewController;
- (NSImage *) smallIcon;
- (NSString *) customerName;

@end
