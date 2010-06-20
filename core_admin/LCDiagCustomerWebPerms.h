//
//  LCDiagCustomerWebPerms.h
//  LCAdminTools
//
//  Created by James Wilson on 5/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagCustomerWebPerms : LCDiagTest 
{
	NSString *customerName;
}

- (LCDiagCustomerWebPerms *) initForCustomerName:(NSString *)name;

@end
