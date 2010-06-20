//
//  LCContentView.m
//  LCAdminTools
//
//  Created by James Wilson on 1/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCContentView.h"


@implementation LCContentView

- (void) drawRect:(NSRect)rect
{
	NSImage *back = [NSImage imageNamed:@"slateback.png"];
	[back drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
			fromRect:NSMakeRect(0,0,[back size].width,[back size].height)
		   operation:NSCompositeSourceOver
			fraction:1.0];
}

@end
