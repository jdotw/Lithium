//
//  LCXSDriveCageView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSDriveCageView.h"


@implementation LCXSDriveCageView

- (LCXSDriveCageView *) initWithFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	return self;
}

- (void) drawRect:(NSRect)rect
{
	NSImage *cageImage = [NSImage imageNamed:@"xsdrivecage.png"];
	[cageImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				 fromRect:NSMakeRect(0,0,[cageImage size].width,[cageImage size].height)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
}

@end
