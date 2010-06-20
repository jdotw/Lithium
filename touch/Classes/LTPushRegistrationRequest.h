//
//  LTPushRegistrationRequest.h
//  Lithium
//
//  Created by James Wilson on 21/06/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTCustomer.h"
#import "LTAPIRequest.h"

@interface LTPushRegistrationRequest : LTAPIRequest 
{
	NSData *token;
	BOOL receiveNotifications;
}

- (id) initWithCustomer:(LTCustomer *)initCustomer token:(NSData *)initToken receiveNotifications:(BOOL)initFlag;
- (void) performRequest;
- (void) performReset;

@property (copy) NSData *token;
@property (assign) BOOL receiveNotifications;


@end
