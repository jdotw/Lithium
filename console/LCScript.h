//
//  LCScript.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCScriptConfigVariable.h"

@interface LCScript : LCXMLObject 
{
	/* Properties */
	NSString *name;
	NSString *desc;
	NSString *info;
	NSString *installedVersion;
	int status;
	NSImage *statusIcon;
	NSString *statusString;
	NSString *type;
	
	/* Config Variables */
 	NSMutableArray *configVariables;
 	NSMutableDictionary *configVariableDict;
}

#pragma mark "Constructors"
- (id) initWithType:(NSString *)initType;

#pragma mark "Properties"
@property (copy) NSString *name;
@property (copy) NSString *desc;
@property (copy) NSString *info;
@property (copy) NSString *installedVersion;
@property (assign) int status;
@property (copy) NSImage *statusIcon;
@property (copy) NSString *statusString;
@property (copy) NSString *type;
@property (readonly) NSMutableArray *configVariables;
- (void) insertObject:(LCScriptConfigVariable *)var inConfigVariablesAtIndex:(unsigned int)index;
- (void) removeObjectFromConfigVariablesAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *configVariableDict;

@end
