//
//  LCLightContentView.m
//  LCAdminTools
//
//  Created by James Wilson on 1/09/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import "LCLightContentView.h"


@implementation LCLightContentView

- (void) drawRect:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:232/256.0 green:232/256.0 blue:232/256.0 alpha:1.0] setFill];
	NSRectFill([self bounds]);	
}

@end
