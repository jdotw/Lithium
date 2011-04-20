//
//  LCDebug.m
//  Lithium Console
//
//  Created by James Wilson on 4/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDebug.h"

void LCDebug (NSString *format, ...)
{
#ifdef DEBUG
	va_list ap;
	va_start (ap, format);
	NSLogv (format, ap);
	va_end (ap);
#endif
}

