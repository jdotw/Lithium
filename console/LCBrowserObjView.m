//
//  LCBrowserObjView.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserObjView.h"
#import "LCRoundedBezierPath.h"

@implementation LCBrowserObjView

#pragma mark "Drawing Method"

- (void) awakeFromNib
{
	[self setAutoresizesSubviews:YES];
}

- (void) drawRect:(NSRect)rect
{
	/* Create outline rect */
	NSRect outlineRect = NSMakeRect([self bounds].origin.x+1, [self bounds].origin.y+12, [self bounds].size.width-13, [self bounds].size.height-13);
	
	/* Create path */
	NSBezierPath *outline = [NSBezierPath bezierPathWithRoundedRect:outlineRect xRadius:5.0 yRadius:5.0];
	
	/* Setup the shadow */
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(5.0, -5.0)]; 
	[theShadow setShadowBlurRadius:8];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.5]];
	[theShadow set];
	
	/* Set colours */
	if ([[self window] isMainWindow])
	{
		[[NSColor colorWithCalibratedRed:36.0/256.0 green:38.0/256.0 blue:49.0/256.0 alpha:1.0] setFill];
		[[NSColor colorWithDeviceRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
	}
	else
	{
		[[NSColor colorWithCalibratedRed:49.0/256.0 green:49.0/256.0 blue:49.0/256.0 alpha:1.0] setFill];
		[[NSColor colorWithDeviceRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
	}
	
	
	/* Fill and stroke */
	[outline fill];
	
	/* Draw clipped image */
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[outline addClip];
	NSImage *bgImage = [NSImage imageNamed:@"browser_objback.png"];
	[bgImage drawInRect:NSMakeRect(0, (NSMaxY([self bounds]) - [bgImage size].height), [bgImage size].width, [bgImage size].height)
			   fromRect:NSMakeRect(2, 0,[bgImage size].width,[bgImage size].height)
			  operation:NSCompositeSourceOver
			   fraction:0.15];	
	[[NSGraphicsContext currentContext] restoreGraphicsState];
	
	/* Cleanup */
	[theShadow release];
}

@end


