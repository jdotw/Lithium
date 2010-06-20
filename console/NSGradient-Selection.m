//
//  NSGradient-Selection.m
//  Lithium Console
//
//  Created by James Wilson on 2/09/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "NSGradient-Selection.h"


@implementation NSGradient (Selection)

+ (NSGradient *) selectionGradientWithAlpha:(float)alpha
{
	NSColor *alternateSelectedControlColor = [NSColor alternateSelectedControlColor];
	CGFloat hue, saturation, brightness, defaultAlpha;
	[[alternateSelectedControlColor colorUsingColorSpaceName:NSDeviceRGBColorSpace] getHue:&hue 
																				saturation:&saturation 
																				brightness:&brightness 
																					 alpha:&defaultAlpha];
	alpha = alpha;
	NSColor *lighterColor = [NSColor colorWithDeviceHue:hue
									saturation:MAX(0.0, saturation-.12) 
									brightness:MIN(1.0, brightness+0.30) 
										 alpha:alpha];
	NSColor *darkerColor = [NSColor colorWithDeviceHue:hue
								   saturation:MIN(1.0, (saturation > .04) ? saturation+0.12 : 0.0) 
								   brightness:MAX(0.0, brightness-0.045) 
										alpha:alpha];
	return [[[NSGradient alloc] initWithStartingColor:lighterColor
										  endingColor:darkerColor] autorelease];	
}

@end
