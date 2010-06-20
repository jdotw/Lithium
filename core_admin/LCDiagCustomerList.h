//
//  LCDiagCustomerList.h
//  LCAdminTools
//
//  Created by James Wilson on 4/10/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagCustomerList : LCDiagTest 
{
	NSMutableArray *customers;
	NSMutableDictionary *curProperties;
	NSURLConnection *urlConn;
	NSMutableData *receivedData;

	NSMutableString *xmlString;
	NSString *xmlElement;
}

@end
