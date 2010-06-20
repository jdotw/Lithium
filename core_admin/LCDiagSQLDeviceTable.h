//
//  LCDiagSQLDeviceTable.h
//  LCAdminTools
//
//  Created by James Wilson on 29/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagSQLDeviceTable : LCDiagTest 
{
	NSString *customerName;
}

- (LCDiagSQLDeviceTable *) initForCustomerNamed:(NSString *)initName;
- (NSString *) customerName;
- (void) setCustomerName:(NSString *)string;

@end
