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
@property (nonatomic,assign) int identifier;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic,assign) BOOL enabled;
@property (nonatomic,assign) int activationMode;
@property (nonatomic,assign) NSTimeInterval delay;
@property (nonatomic,assign) BOOL rerun;	
@property (nonatomic,assign) NSTimeInterval rerunDelay;
@property (nonatomic,assign) BOOL timeFiltered;
@property (nonatomic,assign) int dayMask;
@property (nonatomic,assign) int startHour;
@property (nonatomic,assign) int endHour;
@property (nonatomic,assign) int runCount;
@property (nonatomic,assign) int runState;
@property (nonatomic,copy) NSString *scriptFile;
@property (nonatomic,assign) LTIncident *incident;
@property (nonatomic,assign) int xmlOperationResult;
@property (nonatomic,copy) NSString *xmlOperationMessage;

@end
