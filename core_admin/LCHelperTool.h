//
//  LCHelperTool.h
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <SecurityFoundation/SFAuthorization.h>

@interface LCHelperTool : NSObject 
{
	id delegate;
	float progress;
	NSString *status;
	BOOL successful;
	
	FILE *pipe;
	AuthorizationRef authRef;
	NSFileHandle *readHandle;
}

- (void) startHelperToolCommand:(NSString *)command arguments:(NSArray *)arguments;

@property (assign) id delegate; 
@property (assign) float progress;
@property (copy) NSString *status;
@property (nonatomic, assign) BOOL successful;

- (BOOL) processDataFromHelper:(NSString *)data;
- (void) informDelegteHelperFinished;
- (void) informDelegteHelperFailed;

@end
