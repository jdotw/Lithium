//
//  LCError.m
//  Lithium Console
//
//  Created by James Wilson on 30/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCError.h"

#import "LCErrorLog.h"
#import "LCCustomer.h"

@implementation LCError

+ (LCError *) logError:(NSString *)initError forCustomer:(LCCustomer *)initCustomer fullText:(NSString *)initText
{
	return [[[LCError alloc] initError:initError forCustomer:initCustomer fullText:initText] autorelease];
}

- (LCError *) initError:(NSString *)initError forCustomer:(LCCustomer *)initCustomer fullText:(NSString *)initText
{
	[super init];
	
	date = [[NSDate date] retain];
	error = [initError retain];
	if (initCustomer) customerName = [[initCustomer name] retain];
	if (initText) 
	{
		if ([initText length] > 500)
		{
			fullText = [[NSString stringWithFormat:@"%@...", [initText stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithRange:NSMakeRange(500, [initText length]-500)]]] retain];
		}
		else
		{ fullText = [initText retain]; }
	}
	[[LCErrorLog masterLog] insertObject:self inErrorsAtIndex:0];
	
	return self;
}

- (void) dealloc
{
	[error release];
	[customerName release];
	[fullText release];
	[super dealloc];
}

#pragma mark "Accessors"

- (NSString *) error
{ return error; }

- (NSString *) customerName
{ return customerName; }

- (NSString *) fullText
{ return fullText; }

- (NSDate *) date
{ return date; }

- (NSString *) dateDisplayString
{
	return [date descriptionWithCalendarFormat:@"%H:%M:%S %Y-%m-%d" 
									  timeZone:nil
										locale:nil];
}

@synthesize date;
@synthesize error;
@synthesize customerName;
@synthesize fullText;
@end
