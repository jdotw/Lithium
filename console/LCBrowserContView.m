//
//  LCBrowserContView.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserContView.h"
#import "LCRoundedBezierPath.h"

@implementation LCBrowserContView

#pragma mark "Drawing Method"

- (void) drawRect:(NSRect)rects
{	
	if ([self isHidden]) return;
	
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
 //	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.5] setFill];
//	[[NSColor colorWithCalibratedRed:28/255.0 green:34/255.0 blue:52/255.0 alpha:1.0] setFill];
//	[[NSColor colorWithCalibratedRed:30/255.0 green:30/255.0 blue:30/255.0 alpha:1.0] setFill];
//	[[NSColor colorWithCalibratedRed:20.0/256.0 green:22.0/256.0 blue:35.0/256.0 alpha:1.0] setFill];
	
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
//	[outline stroke];
	
	/* Draw clipped images */
	[[NSGraphicsContext currentContext] saveGraphicsState];
	[outline addClip];
	NSImage *bgImage = [NSImage imageNamed:@"browser_contback.png"];
	[bgImage drawInRect:NSMakeRect(0, NSMaxY([self bounds]) - [bgImage size].height, [self bounds].size.width, [bgImage size].height)
			   fromRect:NSMakeRect(2, 0,[bgImage size].width,[bgImage size].height)
			  operation:NSCompositeSourceOver
			   fraction:0.15];	
//	NSImage *gradImage = [NSImage imageNamed:@"browser_grad.png"];
//	[gradImage drawInRect:NSMakeRect(outlineRect.origin.x, outlineRect.origin.y, outlineRect.size.width, [gradImage size].height * 0.5)
//				 fromRect:NSMakeRect(0, 0,[gradImage size].width,[gradImage size].height)
//				operation:NSCompositeSourceOver
//				 fraction:0.8];		
	[[NSGraphicsContext currentContext] restoreGraphicsState];
	
	[theShadow release];
}

- (void)fadeOut
{
    fadeOutAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObject:[NSDictionary dictionaryWithObjectsAndKeys:self, NSViewAnimationTargetKey, NSViewAnimationFadeOutEffect, NSViewAnimationEffectKey, nil]]];
	[fadeOutAnim setDelegate:self];
    [fadeOutAnim startAnimation];
}

- (void)fadeIn
{
    fadeInAnim = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObject:[NSDictionary dictionaryWithObjectsAndKeys:self, NSViewAnimationTargetKey, NSViewAnimationFadeInEffect, NSViewAnimationEffectKey, nil]]];
	[fadeInAnim setDelegate:self];
    [fadeInAnim startAnimation];
}

- (void)animationDidEnd:(NSAnimation *)animation
{
	if (animation == fadeInAnim) 
	{
		fadeInAnim = nil;
	}
	if (animation == fadeOutAnim) 
	{
		[super setHidden:YES];
		fadeOutAnim = nil;	
	}
    [animation autorelease];
}

- (void)animationDidStop:(NSAnimation *)animation
{
	if (animation == fadeInAnim) 
	{
		fadeInAnim = nil;
	}
	if (animation == fadeOutAnim) 
	{
		[super setHidden:YES];
		fadeOutAnim = nil;	
	}
    [animation autorelease];
}

- (BOOL) isHidden
{ return [super isHidden]; }
- (void) setHidden:(BOOL)flag
{
	if (flag == YES && ![self isHidden])
	{
		/* View is to be hidden */
		if (!initialFadeOutDone)
		{
			/* Performs initial hide without animation */
			[super setHidden:YES];
			initialFadeOutDone = YES;
			return;
		}
		else if (fadeInAnim) [fadeInAnim stopAnimation]; 
		else if (!fadeOutAnim) [self fadeOut];
	}
	else if (flag == NO && [self isHidden])
	{ 
		/* View is to be shown */
		if (fadeOutAnim) [fadeOutAnim stopAnimation];
		if (!fadeInAnim)
		{
			[super setHidden:NO];
			[self fadeIn];
		}
	}
}

@synthesize fadeInAnim;
@synthesize fadeOutAnim;
@synthesize initialFadeOutDone;
@end
