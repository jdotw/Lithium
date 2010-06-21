//
//  LCMetricGraphSet.h
//  Lithium Console
//
//  Created by James Wilson on 6/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntityDescriptor.h"

@interface LCMetricGraphSet : NSObject 
{
	NSMutableArray *metrics;
	NSMutableArray *devices;
	NSMutableDictionary *properties;
}

#pragma mark "Initialisation"
+ (LCMetricGraphSet *) graphSet;
- (LCMetricGraphSet *) init;
- (void) dealloc;

#pragma mark "Accessor Methods"
- (NSMutableDictionary *) properties;
- (NSMutableArray *) devices;
- (NSMutableArray *) metrics;

@property (readonly) NSMutableArray *metrics;
@property (readonly) NSMutableArray *devices;
@property (readonly) NSMutableDictionary *properties;
@property (nonatomic,copy) NSString *desc;

@end
