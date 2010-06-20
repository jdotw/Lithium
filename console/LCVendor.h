//
//  LCVendor.h
//  Lithium Console
//
//  Created by James Wilson on 7/01/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCVendor : NSObject 
{
	NSMutableDictionary *properties;
}

#pragma mark "Constructors"
+ (LCVendor *) vendorWithProperties:(NSMutableDictionary *)initProperties;
- (LCVendor *) initWithProperties:(NSMutableDictionary *)initProperties;
- (LCVendor *) init;
- (void) dealloc;

#pragma mark "Properties Accessors"
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)value;
- (NSString *) name;
- (NSString *) desc;


@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@end
