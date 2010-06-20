//
//  LCDiagTestMarshall.h
//  LCAdminTools
//
//  Created by James Wilson on 28/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCDiagTest.h"

@interface LCDiagTestMarshall : LCDiagTest 
{
	id controller;
}

#pragma mark "Constructors"
- (LCDiagTestMarshall *) initWithController:(id)initController;

- (id) controller;
- (void) setController:(id)newController;


@end
