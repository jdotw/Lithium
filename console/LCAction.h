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
	BOOL logOutput;
	
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
@property (nonatomic, assign) id updateDelegate;

#pragma mark "Execute Script"
- (void) execute;
@property (nonatomic, assign) id executeDelegate;
@property (nonatomic,copy) NSString *executeStatusString;
@property (nonatomic,copy) NSString *executeScriptOutput;
@property (nonatomic,copy) NSImage *executeStatusIcon;

#pragma mark "Day Accessors"
@property (nonatomic, assign) BOOL monday;
@property (nonatomic, assign) BOOL tuesday;
@property (nonatomic, assign) BOOL wednesday;
@property (nonatomic, assign) BOOL thursday;
@property (nonatomic, assign) BOOL friday;
@property (nonatomic, assign) BOOL saturday;
@property (nonatomic, assign) BOOL sunday;
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
@property (nonatomic, assign) int activationMode;
@property (nonatomic, assign) int delay;
@property (nonatomic, assign) BOOL willReRun;
@property (nonatomic, assign) int reRunDelay;
@property (nonatomic, assign) BOOL timeFiltered;
@property (nonatomic, assign) int dayMask;
@property (nonatomic, assign) int startHour;
@property (nonatomic, assign) int endHour;
@property (nonatomic, assign) int reRunCount;
@property (nonatomic, assign) int runState;
@property (nonatomic, assign) unsigned long runStartTimestamp;
@property (nonatomic, assign) unsigned long runEndTimestamp;
@property (nonatomic, assign) BOOL logOutput;

#pragma mark "Dyanmic Property Methods"
@property (nonatomic,copy) NSImage *actionIcon;
@property (nonatomic,copy) NSString *executionString;
- (void) updateExecutionString;
@property (nonatomic,copy) NSString *rerunString;
- (void) updateRerunString;
@property (nonatomic,copy) NSString *behaviourString;
- (void) updateBehaviourString;
@property (nonatomic,copy) NSString *timeToExecutionString;
- (void) updateTimeToExecutionString;

#pragma mark "Other Property Methods"
@property (nonatomic, assign) id incident;
@property (nonatomic,retain) LCActionEntityList *entityList;
@property (nonatomic,retain) LCActionHistoryList *historyList;

@end
