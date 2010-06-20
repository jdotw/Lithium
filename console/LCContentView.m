//
//  LCContentView.m
//  Lithium Console
//
//  Created by James Wilson on 24/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCContentView.h"
#import "LCRoundedBezierPath.h"

@implementation LCContentView

#pragma mark "Drawing Method"

- (void) awakeFromNib
{
	[self setBackImage:[NSImage imageNamed:@"swishIdea1.png"]];
}

- (void) drawRect:(NSRect)rect
{
	/* Create outline rect */
	NSRect outlineRect = NSMakeRect([self bounds].origin.x+1, [self bounds].origin.y+12, [self bounds].size.width-13, [self bounds].size.height-13);
	
	/* Create path */
	NSBezierPath *outline = [LCRoundedBezierPath pathInRect:outlineRect];
	
	/* Setup the shadow */
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(5.0, -5.0)]; 
	[theShadow setShadowBlurRadius:5];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3]];
	[theShadow set];
	
	/* Set colours */
//	[[NSColor colorWithCalibratedRed:26.0/256.0 green:27.0/256.0 blue:40.0/256.0 alpha:1.0] setFill];
//	[[NSColor colorWithCalibratedRed:32/255.0 green:38/255.0 blue:55/255.0 alpha:1.0] setFill];
	[[NSColor colorWithCalibratedRed:36.0/256.0 green:38.0/256.0 blue:49.0/256.0 alpha:1.0] setFill];
	[[NSColor colorWithDeviceRed:1.0 green:1.0 blue:1.0 alpha:0.2] setStroke];
	
	/* Fill and stroke */
	[outline fill];
	
	/* Draw clipped image */
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[outline addClip];
	if (backImage)
	{
		[backImage drawInRect:NSMakeRect(0, NSMaxY([self bounds]) - [backImage size].height, [backImage size].width, [backImage size].height)
				   fromRect:NSMakeRect(2, 0,[backImage size].width,[backImage size].height)
				  operation:NSCompositeSourceOver
				   fraction:0.15];

	}

//	[outline addClip];
//	backImage = [NSImage imageNamed:@"browsercontentback.tif"];
//	if (backImage)
//	{
//		[backImage drawInRect:NSMakeRect(0, 0, [self bounds].size.width, [self bounds].size.height)
//					 fromRect:NSMakeRect(2, 0,[backImage size].width,[backImage size].height)
//					operation:NSCompositeSourceOver
//					 fraction:1.0];
//	}
	
//	NSImage *gradImage = [NSImage imageNamed:@"content_grad.png"];
//	[gradImage drawInRect:NSMakeRect(outlineRect.origin.x, outlineRect.origin.y, outlineRect.size.width, [gradImage size].height)
//				 fromRect:NSMakeRect(0, 0,[gradImage size].width,[gradImage size].height)
//				operation:NSCompositeSourceOver
//				 fraction:1.0];		
	
	[[NSGraphicsContext currentContext] restoreGraphicsState];
	
	[theShadow release];
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
}

- (NSImage *) backImage
{ return backImage; }

- (void) setBackImage:(NSImage *)value
{
	[backImage release];
	backImage = [value retain];
}

@end
