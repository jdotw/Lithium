//
//  LCDiagSQLSiteTable.h
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagSQLSiteTable : LCDiagTest 
{
	NSString *customerName;
}

- (NSString *) customerName;
- (void) setCustomerName:(NSString *)string;

@end
