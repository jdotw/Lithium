//
//  LTTableViewCellSelectedBackground.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewCellSelectedBackground.h"


@implementation LTTableViewCellSelectedBackground


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
    }
    return self;
}


- (void)drawRect:(CGRect)rect 
{
    // Drawing code
	[[UIColor colorWithRed:1.0/255.0 green:99.0/255.0 blue:232.0/255.0 alpha:1.0] setFill];
	UIRectFill(self.bounds);
	
	// Ridge lines
	[[UIColor colorWithRed:0.1 green:0.1 blue:0.1 alpha:1.0] setFill];
	CGRect topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-1.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);
	[[UIColor colorWithRed:76.0/255.0 green:76.0/255.0 blue:76.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds)-1.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);
	[[UIColor colorWithRed:60.0/255.0 green:60.0/255.0 blue:60.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMinY(self.bounds)+0.0, CGRectGetWidth(self.bounds), 1.0);
	UIRectFill(topLineRect);
	
	// Gradient
	UIImage *image = [UIImage imageNamed:@"CellLowerGradient.png"];
	[image drawInRect:CGRectMake(CGRectGetMinX(self.bounds), CGRectGetMaxY(self.bounds)-6.0, CGRectGetWidth(self.bounds), 6.0)
			blendMode:kCGBlendModeSourceAtop 
				alpha:0.3];
}


- (void)dealloc {
    [super dealloc];
}


@end
