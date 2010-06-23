//
//  MBOIDCell.m
//  ModuleBuilder
//
//  Created by James Wilson on 25/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MBOIDCell.h"


@implementation MBOIDCell

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	NSImage *image;
	if (drawnOid)
	{
		image = [NSImage imageNamed:@"marker_16_flipped.png"];
	}
	else
	{
		image = [NSImage imageNamed:@"product_16_flipped.png"];
	}
	[image setFlipped:YES];
	[image drawInRect:NSMakeRect(NSMinX(cellFrame)+3, NSMinY(cellFrame)+5, 14.0, 14.0)
			 fromRect:NSMakeRect(0.0, 0.0, 16.0, 16.0) 
			operation:NSCompositeSourceOver
			 fraction:1.0];

	NSRect indentedCellFrame = NSMakeRect(NSMinX(cellFrame)+18, NSMinY(cellFrame), NSWidth(cellFrame), NSHeight(cellFrame));
	[super drawInteriorWithFrame:indentedCellFrame inView:controlView];
}

@synthesize drawnOid;

@end
