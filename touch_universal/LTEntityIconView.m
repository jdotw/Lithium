//
//  LTEntityIconView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTEntityIconView.h"


@implementation LTEntityIconView

@synthesize selected, selectedBackgroundImage;

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code.
    }
    return self;
}

- (void)drawRect:(CGRect)rect 
{	
	/* Selected overlay wash */
	if (self.selected && self.selectedBackgroundImage)
	{
		CGRect highlightRect = self.bounds;
		highlightRect.size.width -= 2.;
		[self.selectedBackgroundImage drawInRect:highlightRect];		
	}
	
	/* Draw right-side edge */
	CGRect vertBorderRect = CGRectMake(CGRectGetMaxX(self.bounds)-2.0, CGRectGetMinY(self.bounds), 1.0, CGRectGetHeight(self.bounds));
	[[UIColor colorWithWhite:0.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:vertBorderRect] fillWithBlendMode:kCGBlendModeDarken alpha:0.3];
	[[UIColor colorWithWhite:1.0 alpha:1.0] setFill];
	[[UIBezierPath bezierPathWithRect:CGRectOffset(vertBorderRect, 1.0, 0.0)] fillWithBlendMode:kCGBlendModeDarken alpha:0.25];

}

- (void)dealloc {
    [super dealloc];
}


@end
