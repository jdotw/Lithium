//
//  LCBlankDeviceView.m
//  Lithium Console
//
//  Created by James Wilson on 21/07/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBlankDeviceView.h"


@implementation LCBlankDeviceView

#pragma mark "Init"

- (LCBlankDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame
{
	[super initWithFrame:frame];
	
	device = [initDevice retain];

	/* Add observers */
	[device addObserver:self 
			 forKeyPath:@"refreshInProgress" 
				options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				context:NULL];
	
	return self;
}

#pragma mark "View Management"

- (void) removeFromSuperview
{
	/* Remove observer */
	[device removeObserver:self forKeyPath:@"refreshInProgress"];
	
	[super removeFromSuperview];
}

#pragma mark "KVO"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	[self setNeedsDisplay:YES];
}

#pragma mark "Drawing"

- (void) drawRect:(NSRect)rect
{
	if ([self drawWarnings]) return;
	
	NSShadow* theShadow = [[NSShadow alloc] init]; 
	[theShadow setShadowOffset:NSMakeSize(2.0, -2.0)]; 
	[theShadow setShadowBlurRadius:1.0];
	[theShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.35]];
	[theShadow set];	
	NSString *str = @"";
	NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSColor colorWithCalibratedRed:0.98 green:0.98 blue:0.98 alpha:1.0], NSForegroundColorAttributeName,
		[NSFont fontWithName:@"Lucida Grande Bold" size:18.0], NSFontAttributeName,
		nil];
	NSSize textSize = [str sizeWithAttributes:attr];
	NSRect textRect = NSMakeRect(NSMidX([self bounds]) - (textSize.width * 0.5), 
								 NSMidY([self bounds]) - (textSize.height * 0.2),
								 textSize.width, textSize.height);
	[str drawInRect:textRect withAttributes:attr];
	[theShadow release];
	
	[super drawRect:rect];
}

@end
