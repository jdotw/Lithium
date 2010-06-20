//
//  LCXSPowerVisorView.m
//  Lithium Console
//
//  Created by James Wilson on 12/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSPowerVisorView.h"


@implementation LCXSPowerVisorView

- (LCXSPowerVisorView *) initWithFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	return self;
}

- (void) drawRect:(NSRect)rect
{
	NSRect visorRect = [self bounds];
	NSBezierPath *visorPath = [NSBezierPath bezierPathWithRect:visorRect];
	[[NSColor colorWithCalibratedWhite:0.1 alpha:0.8] setFill];
	[visorPath fill];
	
	NSShadow *textShadow = [[NSShadow alloc] init]; 
	[textShadow setShadowOffset:NSMakeSize(3.0, -3.0)]; 
	[textShadow setShadowBlurRadius:3];
	[textShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.5]];				
	NSString *str = @"System Power Off";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithDeviceRed:1.0 green:1.0 blue:1.0 alpha:0.9], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande" size:16.0], NSFontAttributeName,
		nil];		
	NSSize strSize = [str sizeWithAttributes:attr];
	NSPoint strPoint = NSMakePoint(NSMidX([self bounds]) - (strSize.width / 2), NSMidY([self bounds]) - (strSize.height / 2));
	[str drawAtPoint:strPoint withAttributes:attr];
	[textShadow release];
}

- (void) fadeIn
{
	if ([self isHidden])
	{
		/* Dev view resize */
		NSDictionary *view1Resize;
		view1Resize = [NSDictionary dictionaryWithObjectsAndKeys:
			self, NSViewAnimationTargetKey, 
			NSViewAnimationFadeInEffect,
			NSViewAnimationEffectKey,
			nil];
		
		/* Resize the dev container view */
		NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: view1Resize, nil]];
		[animation setDuration:1.2];
		[animation setDelegate:self];
		[animation startAnimation];
		
		[animation release];	
	}
}

- (void) fadeOut
{
	if (![self isHidden])
	{
		/* Dev view resize */
		NSDictionary *view1Resize;
		view1Resize = [NSDictionary dictionaryWithObjectsAndKeys:
			self, NSViewAnimationTargetKey, 
			NSViewAnimationFadeOutEffect,
			NSViewAnimationEffectKey,
			nil];
		
		/* Resize the dev container view */
		NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: view1Resize, nil]];
		[animation setDuration:1.2];
		[animation setDelegate:self];
		[animation startAnimation];
		
		[animation release];	
	}
}

@end
