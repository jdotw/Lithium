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
@property (nonatomic, assign) unsigned long incidentID;
@property (nonatomic, assign) unsigned long caseID;
@property (nonatomic, assign) int state;
@property (nonatomic, assign) int type;
@property (nonatomic, assign) BOOL isActive;
@property (nonatomic, assign) BOOL isLive;
@property (nonatomic, assign) BOOL hasCase;
@property (nonatomic, assign) unsigned long startTimestamp;
@property (nonatomic,copy) NSDate *startDate;
@property (nonatomic,copy) NSString *startDateShortString;
@property (nonatomic, assign) unsigned long endTimestamp;
@property (nonatomic,copy) NSDate *endDate;
@property (nonatomic,copy) NSString *endDateShortString;
@property (nonatomic, assign) NSTimeInterval openTimeInterval;
- (void) updateOpenTimeInterval;
@property (nonatomic,copy) NSString *raisedValueString;
@property (nonatomic,copy) NSString *clearedValueString;
@property (nonatomic,retain) LCEntityDescriptor *entityDescriptor;
@property (nonatomic,retain) LCEntity *entity;
@property (nonatomic, assign) int relevanceScore;
@property (nonatomic, assign) BOOL relatedSelected;
@property (nonatomic, assign) BOOL relatedSelectionDisabled;

#pragma mark "General Accessors"
- (NSArray *) incidents;
- (id) viewController;
- (NSImage *) smallIcon;
- (NSString *) customerName;

@end
