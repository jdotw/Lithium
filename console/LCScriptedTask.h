//
//  LCScriptedTask.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCEntity.h"
#import "LCScript.h"
#import "LCScriptList.h"
#import "LCScriptConfigVariableList.h"
#import "LCCustomer.h"

@interface LCScriptedTask : LCXMLObject
{
	/* Task Properties */
	int taskID;
	NSString *desc;
	BOOL enabled;
	NSString *scriptName;
	
	/* Config */
	NSString *taskType;			/* action/service */

	/* Related Objects */
	LCEntity *hostEntity;		/* The host of the task (Device/Customer) */
	LCCustomer *customer;		/* Kept in sync with hostEntity.customer */
	
	/* Scripts */
	LCScriptList *scriptList;
	LCScript *selectedScript;
	
	/* Config Variables */
	LCScriptConfigVariableList *configVariables;
}

#pragma mark "Constructors"
- (id) initWithHostEntity:(LCEntity *)initHostEntity
				 taskType:(NSString *)initType;

#pragma mark "Properties"

@property (assign) int taskID;
@property (copy) NSString *desc;
@property (assign) BOOL enabled;
@property (copy) NSString *scriptName;

@property (copy) NSString *taskType;

@property (retain) LCEntity *hostEntity;
@property (retain) LCCustomer *customer;

@property (retain) LCScriptList *scriptList;
@property (assign) LCScript *selectedScript;

@property (retain) LCScriptConfigVariableList *configVariables;

@end


