//
//  LCWindow.m
//  Lithium Console
//
//  Created by James Wilson on 8/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCWindow.h"


@implementation LCWindow

- (BOOL) makeFirstResponder:(NSResponder *)aResponder
{
	BOOL result = [super makeFirstResponder:aResponder];
	if ([[self delegate] respondsToSelector:@selector(windowFirstResponderDidChange:)])
	{ [[self delegate] performSelector:@selector(windowFirstResponderDidChange:)
							withObject:aResponder]; }
	return result; 
}

@end
