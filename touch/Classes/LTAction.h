//
//  LTAction.h
//  Lithium
//
//  Created by James Wilson on 1/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTIncident.h"
#import "LTAPIRequest.h"

@interface LTAction : LTAPIRequest <NSXMLParserDelegate>
{
	/* Action Properties */
	int identifier;
	NSString *desc;
	BOOL enabled;
	int activationMode;
	NSTimeInterval delay;
	BOOL rerun;	
	NSTimeInterval rerunDelay;
	BOOL timeFiltered;
	int dayMask;
	int startHour;
	int endHour;
	int runCount;
	int runState;
	NSString *scriptFile;
	
	/* Incident */
	LTIncident *incident;
	
	/* Execute Properties */
	BOOL xmlOperationInProgress;
	NSMutableString *curXmlString;
	int xmlOperationResult;
	NSString *xmlOperationMessage;
}

#pragma mark "Execution"
- (void) execute;
- (void) cancel;

#pragma mark "Properties"
@property (assign) int identifier;
@property (copy) NSString *desc;
@property (assign) BOOL enabled;
@property (assign) int activationMode;
@property (assign) NSTimeInterval delay;
@property (assign) BOOL rerun;	
@property (assign) NSTimeInterval rerunDelay;
@property (assign) BOOL timeFiltered;
@property (assign) int dayMask;
@property (assign) int startHour;
@property (assign) int endHour;
@property (assign) int runCount;
@property (assign) int runState;
@property (copy) NSString *scriptFile;
@property (assign) LTIncident *incident;
@property (assign) int xmlOperationResult;
@property (copy) NSString *xmlOperationMessage;

@end
