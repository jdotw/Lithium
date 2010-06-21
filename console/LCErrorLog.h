//
//  LCErrorLog.h
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCError.h"

@interface LCErrorLog : NSObject 
{
	NSMutableArray *errors;
}

#pragma mark "Constructors"
+ (LCErrorLog *) masterInit;
- (LCErrorLog *) init;
- (void) dealloc;

#pragma mark "Accessors"
- (NSMutableArray *) errors;
- (void) insertObject:(LCError *)error inErrorsAtIndex:(unsigned int)index;
- (void) removeObjectFromErrorsAtIndex:(unsigned int)index;
+ (NSMutableArray *) masterErrors;
+ (LCErrorLog *) masterLog;
- (void) clearLog;

@property (nonatomic,retain,getter=errors) NSMutableArray *errors;
@end
