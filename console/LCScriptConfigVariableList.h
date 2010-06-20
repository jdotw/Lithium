//
//  LCScriptConfigVariableList.h
//  Lithium Console
//
//  Created by James Wilson on 10/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCXMLRequest.h"

@interface LCScriptConfigVariableList : NSObject 
{
	/* Related Objects */
	id task;
	
	/* Variables */
	NSMutableDictionary *variableDict;
	
	/* XML Variables */
	LCXMLRequest *refreshXMLRequest;
	BOOL refreshInProgress;
}

#pragma mark "Refresh methods"
- (void) refreshWithPriority:(int)priority;
- (void) highPriorityRefresh;
- (void) normalPriorityRefresh;
- (void) lowPriorityRefresh;

#pragma mark "Script Update"
- (void) updateScriptConfigVariables:(id)script;

#pragma mark "Accessor Methods"
@property (assign) id task;
@property (readonly) NSMutableDictionary *variableDict;
@property (assign) BOOL refreshInProgress;

@end
