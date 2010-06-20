//
//  LCTextFieldCell.m
//  Lithium Console
//
//  Created by James Wilson on 8/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCTextFieldCell.h"


@implementation LCTextFieldCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	/* Establish size of text */
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys: [self font], NSFontAttributeName, nil];
	NSSize textSize = [[self stringValue] sizeWithAttributes:attr];
	
	/* Create centered rect */
	NSRect centeredRect = NSMakeRect(cellFrame.origin.x + 1,
									 (cellFrame.origin.y + (0.5 * (cellFrame.size.height - textSize.height))) + 1,
									 cellFrame.size.width - 1,
									 textSize.height);
	
	/* Call super */
	[super drawInteriorWithFrame:centeredRect inView:controlView];
}

@end
