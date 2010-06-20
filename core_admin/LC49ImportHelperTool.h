//
//  LC49ImportHelperTool.h
//  LCAdminTools
//
//  Created by James Wilson on 2/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCHelperTool.h"

#define OP_L49DB_START 1
#define OP_L49DB_STOP 2 
#define OP_L49DB_EXPORT 3

#define OP_L50DB_START 11
#define OP_L50DB_IMPORT 12
#define OP_L50DB_STOP 13

#define OP_L49RRD_COPY 21

#define OP_L49CUST_CREATE 31

#define OP_ALL_STOP 41
#define OP_ALL_START 42

#define OP_L49ROOT_RENAME 51
#define OP_L49ROOT_REINSTATE 52

#define OP_FINALIZE 61

@interface LC49ImportHelperTool : LCHelperTool 
{
	int operation;
}

- (id) initWithDelegate:(id)initDelegate;
- (void) reinstateL49Root;
- (void) startL49Database;
- (void) exportL49Database;
- (void) stopL49Database;
- (void) startL50Database;
- (void) importL50Database;
- (void) stopL50Database;
- (void) copyL49RRDFiles;
- (void) copyL49ClientFiles;
- (void) allStop;
- (void) allStart;
- (void) renameL49Root;
- (void) finalizeImport;
	
@end
