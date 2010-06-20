//
//  LCDiagCustomerAuth.h
//  LCAdminTools
//
//  Created by James Wilson on 4/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagCustomerAuth : LCDiagTest 
{
	NSString *customerName;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;
}

- (LCDiagCustomerAuth *) initForCustomerName:(NSString *)name;

@end
