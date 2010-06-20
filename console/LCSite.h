//
//  LCSite.h
//  Lithium Console
//
//  Created by James Wilson on 20/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"

@interface LCSite : LCEntity 
{
	NSString *addressLine1;
	NSString *addressLine2;
	NSString *addressLine3;
	NSString *suburb;
	NSString *state;
	NSString *postcode;
	NSString *country;
	float longitude;
	float latitude;
}

- (LCSite *) init;
- (void) dealloc;
- (Class) childClass;

@property (retain) NSString *addressLine1;
@property (retain) NSString *addressLine2;
@property (retain) NSString *addressLine3;
@property (retain) NSString *suburb;
@property (retain) NSString *state;
@property (retain) NSString *postcode;
@property (retain) NSString *country;
@property (assign) float longitude;
@property (assign) float latitude;

@end
