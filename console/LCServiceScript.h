//
//  LCServiceScript.h
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCScript.h"

@interface LCServiceScript : LCScript 
{
	/* Related Objects */
	id service;
	
	/* Script Properties */
	int port;
	NSString *protocol;
	NSString *transport;
	
}

#pragma mark "Properties"
@property (nonatomic, assign) int port;
@property (nonatomic,copy) NSString *protocol;
@property (nonatomic,copy) NSString *transport;
@end
