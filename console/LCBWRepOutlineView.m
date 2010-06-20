//
//  LCBWRepOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 28/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBWRepOutlineView.h"
#import "LCBWRepItemTreeController.h"

@implementation LCBWRepOutlineView

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	/* Call super to handle selection */
	[super menuForEvent:event];

	return [(LCBWRepItemTreeController *)[self delegate] menuForEvent:event];
}

@end
