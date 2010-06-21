//
//  LCScriptList.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCScript.h"
#import "LCCustomer.h"
#import "LCXMLRequest.h"

@interface LCScriptList : NSObject 
{
	/* Related Objects */
	LCCustomer *customer;
	id task;
	id delegate;
	
	/* Config */
	NSString *scriptType;
	
	/* Scripts */
	NSMutableArray *scripts;
	NSMutableDictionary *scriptDict;
	
	/* XML Operations */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;	
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer task:(id)initTask;

#pragma mark "List refresh"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Sub-Class Methods (to be overidden)"
- (LCScript *) newScript;
- (NSString *) scriptType;

#pragma mark "Properties"
@property (nonatomic,retain) LCCustomer *customer;
@property (nonatomic, assign) id task;
@property (nonatomic, assign) id delegate;
@property (nonatomic,copy) NSString *scriptType;
@property (readonly) NSMutableArray *scripts;
- (void) insertObject:(LCScript *)script inScriptsAtIndex:(unsigned int)index;
- (void) removeObjectFromScriptsAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *scriptDict;
@property (nonatomic, assign) BOOL refreshInProgress;


@end
