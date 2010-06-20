//
//  LCTriggersetAppliedTransformer.h
//  Lithium Console
//
//  Created by James Wilson on 8/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCTriggerset.h"

@interface LCTriggersetAppliedTransformer : NSValueTransformer 
{
	
}

+ (Class) transformedValueClass;
+ (BOOL) allowsReverseTransformation;
- (NSImage *) transformedValue:(NSString *)value;

@end
