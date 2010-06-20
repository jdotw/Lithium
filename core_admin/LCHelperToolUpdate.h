//
//  LCHelperToolUpdate.h
//  LCAdminTools
//
//  Created by James Wilson on 12/05/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCHelperTool.h"

@interface LCHelperToolUpdate : LCHelperTool 
{

}

- (id) initWithArchive:(NSString *)initArchive extractTo:(NSString *)initExtract containingPackage:(NSString *)initPackage delegate:(id)initDelegate;
- (BOOL) processDataFromHelper:(NSString *)data;

@end
