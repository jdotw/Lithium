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

@property (nonatomic,retain) NSString *addressLine1;
@property (nonatomic,retain) NSString *addressLine2;
@property (nonatomic,retain) NSString *addressLine3;
@property (nonatomic,retain) NSString *suburb;
@property (nonatomic,retain) NSString *state;
@property (nonatomic,retain) NSString *postcode;
@property (nonatomic,retain) NSString *country;
@property (nonatomic, assign) float longitude;
@property (nonatomic, assign) float latitude;

@end
