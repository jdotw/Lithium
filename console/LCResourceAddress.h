//
//  LCResourceAddress.h
//  Lithium Console
//
//  Created by James Wilson on 21/06/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCResourceAddress : NSObject 
{
	/* Properties */
	NSMutableDictionary *properties;
}

#pragma mark "Initialisation"
+ (LCResourceAddress *) addressWithString:(NSString *)string;
+ (LCResourceAddress *) address;
- (LCResourceAddress *) initWithString:(NSString *)string;
- (LCResourceAddress *) init;

#pragma mark "Address String Methods"
- (NSString *) addressString;

#pragma mark "Accessor methods"
@property (readonly) NSMutableDictionary *properties;
- (NSString *) cluster;
- (NSString *) node;
- (NSString *) typeInteger;
- (NSString *) idString;
- (NSString *) idInteger;

@end
