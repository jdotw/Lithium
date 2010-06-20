//
//  LCXSLidView.m
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCXSLidView.h"


@implementation LCXSLidView

- (LCXSLidView *) initWithFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	return self;
}

- (void) drawRect:(NSRect)rect
{
	NSImage *lidImage = [NSImage imageNamed:@"xslid.png"];
	[lidImage drawInRect:NSMakeRect(0,0,NSWidth([self bounds]),NSHeight([self bounds]))
				 fromRect:NSMakeRect(0,0,[lidImage size].width,[lidImage size].height)
				operation:NSCompositeSourceOver 
				 fraction:1.0];
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
