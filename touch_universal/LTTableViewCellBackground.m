//
//  LTTableViewCellBackground.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewCellBackground.h"


@implementation LTTableViewCellBackground


- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
	{
		
    }
    return self;
}


- (void)drawRect:(CGRect)rect 
{
	return;
    // Drawing code
	[[UIColor colorWithRed:70.0/255.0 green:70.0/255.0 blue:70.0/255.0 alpha:1.0] setFill];
	UIRectFill(self.bounds);
	
	// Indent offset 
//	CGFloat indentOffset = cell.indentationLevel * cell.indentationWidth;
	CGFloat indentOffset = 0.0;
	
	// Ridge lines
	CGRect topLineRect;
	
	// Bottom (Dark)
	[[UIColor colorWithRed:20.0/255.0 green:20.0/255.0 blue:20.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds)+indentOffset, CGRectGetMaxY(self.bounds)-1.0, 
							 CGRectGetWidth(self.bounds)-indentOffset, 1.0);
	UIRectFill(topLineRect);
	
	// Top (Light)
	[[UIColor colorWithRed:120.0/255.0 green:120.0/255.0 blue:120.0/255.0 alpha:1.0] setFill];
	topLineRect = CGRectMake(CGRectGetMinX(self.bounds)+indentOffset, CGRectGetMinY(self.bounds)+0.0, 
							 CGRectGetWidth(self.bounds)-indentOffset, 1.0);
	UIRectFill(topLineRect);
	
	// Gradient
	UIImage *image = [UIImage imageNamed:@"CellLowerGradient.png"];
	[image drawInRect:CGRectMake(CGRectGetMinX(self.bounds)+indentOffset, CGRectGetMaxY(self.bounds)-6.0, 
								 CGRectGetWidth(self.bounds)-indentOffset, 6.0)
			blendMode:kCGBlendModeSourceAtop 
				alpha:0.2];
}


- (void)dealloc {
    [super dealloc];
}

@synthesize cell;

@end
