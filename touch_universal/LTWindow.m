//
//  LTWindow.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTWindow.h"


@implementation LTWindow

- (void) drawRect:(CGRect)rect
{
	UIImage *image = [UIImage imageNamed:@"noiseback-light.png"];
	[image drawInRect:self.bounds blendMode:kCGBlendModeSourceAtop alpha:0.1];
}

@end
