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

@property (nonatomic, assign) int taskID;
@property (nonatomic,copy) NSString *desc;
@property (nonatomic, assign) BOOL enabled;
@property (nonatomic,copy) NSString *scriptName;

@property (nonatomic,copy) NSString *taskType;

@property (nonatomic,retain) LCEntity *hostEntity;
@property (nonatomic,retain) LCCustomer *customer;

@property (nonatomic,retain) LCScriptList *scriptList;
@property (nonatomic, assign) LCScript *selectedScript;

@property (nonatomic,retain) LCScriptConfigVariableList *configVariables;

@end


