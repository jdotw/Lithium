//
//  LCAction.h
//  Lithium Console
//
//  Created by James Wilson on 1/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCScriptedTask.h"
#import "LCXMLObject.h"
#import "LCActionScriptList.h"
#import "LCActionScript.h"
#import "LCActionEntityList.h"
#import "LCActionHistoryList.h"

@interface LCAction : LCScriptedTask 
{
	/* Incident Properties */
	int activationMode;
	int delay;
	BOOL willReRun;
	int reRunDelay;
	BOOL timeFiltered;
	int dayMask;
	int startHour;
	int endHour;
	int reRunCount;
	int runState;
	unsigned long runStartTimestamp;
	unsigned long runEndTimestamp;
	
	/* Dynamic properties */
	NSString *executionString;
	NSString *rerunString;
	NSString *behaviourString;
	NSString *timeToExecutionString;
	NSImage *actionIcon;
	
	/* Related Objects */
	id incident;
	
	/* Entity List */
	LCActionEntityList *entityList;
	
	/* Log Entries */
	LCActionHistoryList *historyList;
	
	/* Script execution */
	LCXMLRequest *executeRequest;
	NSString *executeStatusString;
	NSString *executeScriptOutput;
	NSImage *executeStatusIcon;
	id executeDelegate;
	
	/* XML Ops */
	LCXMLRequest *xmlReq;
	id updateDelegate;
}

#define ACTION_DAY_MON 1
#define ACTION_DAY_TUE 2
#define ACTION_DAY_WED 4
#define ACTION_DAY_THU 8
#define ACTION_DAY_FRI 16
#define ACTION_DAY_SAT 32
#define ACTION_DAY_SUN 64

#pragma mark "XML Methods"
@property (assign) id updateDelegate;

#pragma mark "Execute Script"
- (void) execute;
@property (assign) id executeDelegate;
@property (copy) NSString *executeStatusString;
@property (copy) NSString *executeScriptOutput;
@property (copy) NSImage *executeStatusIcon;

#pragma mark "Day Accessors"
@property (assign) BOOL monday;
@property (assign) BOOL tuesday;
@property (assign) BOOL wednesday;
@property (assign) BOOL thursday;
@property (assign) BOOL friday;
@property (assign) BOOL saturday;
@property (assign) BOOL sunday;
- (BOOL) monday;
- (void) setMonday:(BOOL)value;
- (BOOL) tuesday;
- (void) setTuesday:(BOOL)value;
- (BOOL) wednesday;
- (void) setWednesday:(BOOL)value;
- (BOOL) thursday;
- (void) setThursday:(BOOL)value;
- (BOOL) friday;
- (void) setFriday:(BOOL)value;
- (BOOL) saturday;
- (void) setSaturday:(BOOL)value;
- (BOOL) sunday;
- (void) setSunday:(BOOL)value;

#pragma mark "Action Property Methods"
@property (assign) int activationMode;
@property (assign) int delay;
@property (assign) BOOL willReRun;
@property (assign) int reRunDelay;
@property (assign) BOOL timeFiltered;
@property (assign) int dayMask;
@property (assign) int startHour;
@property (assign) int endHour;
@property (assign) int reRunCount;
@property (assign) int runState;
@property (assign) unsigned long runStartTimestamp;
@property (assign) unsigned long runEndTimestamp;

#pragma mark "Dyanmic Property Methods"
@property (copy) NSImage *actionIcon;
@property (copy) NSString *executionString;
- (void) updateExecutionString;
@property (copy) NSString *rerunString;
- (void) updateRerunString;
@property (copy) NSString *behaviourString;
- (void) updateBehaviourString;
@property (copy) NSString *timeToExecutionString;
- (void) updateTimeToExecutionString;

#pragma mark "Other Property Methods"
@property (assign) id incident;
@property (retain) LCActionEntityList *entityList;
@property (retain) LCActionHistoryList *historyList;

@end
