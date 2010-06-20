//
//  LCModuleHelper.h
//  LCAdminTools
//
//  Created by James Wilson on 16/03/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCHelperTool.h"

#define OP_INSTALL 10
#define OP_DELETE 2

@interface LCModuleHelper : LCHelperTool 
{
	int operation;
}

- (void) installModule:(NSString *)fullpath filename:(NSString *)filename;
- (void) deleteModule:(NSString *)filename;

@end
