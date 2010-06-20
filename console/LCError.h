//
//  LCError.h
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCCustomer.h"

@interface LCError : NSObject 
{
	NSDate *date;
	NSString *error;
	NSString *customerName;
	
	NSString *fullText;
}

+ (LCError *) logError:(NSString *)initError forCustomer:(LCCustomer *)initCustomer fullText:(NSString *)initText;
- (LCError *) initError:(NSString *)initError forCustomer:(LCCustomer *)initCustomer fullText:(NSString *)initText;
- (void) dealloc;

#pragma mark "Accessors"
- (NSString *) error;
- (NSString *) customerName;
- (NSString *) fullText;
- (NSDate *) date;


@property (retain,getter=date) NSDate *date;
@property (retain,getter=error) NSString *error;
@property (retain,getter=customerName) NSString *customerName;
@property (retain,getter=fullText) NSString *fullText;
@end
