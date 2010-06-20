//
//  LCCoreInfoResource.h
//  Lithium Console
//
//  Created by James Wilson on 26/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCCoreInfoResource : NSObject 
{
	NSMutableDictionary *properties;
}

+ (LCCoreInfoResource *) resourceWithProperties:(NSMutableDictionary *)initProperties;
- (LCCoreInfoResource *) initWithProperties:(NSMutableDictionary *)initProperties;
- (NSMutableDictionary *) properties;
- (void) setProperties:(NSMutableDictionary *)dict;
- (NSString *) id;

@property (retain,getter=properties,setter=setProperties:) NSMutableDictionary *properties;
@end
