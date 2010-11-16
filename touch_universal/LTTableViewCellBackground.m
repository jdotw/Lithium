//
//  LTTableViewCellBackground.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableViewCellBackground.h"
#import <QuartzCore/QuartzCore.h>

@implementation LTTableViewCellBackground

@synthesize entityState, cell;

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
	{
		
    }
    return self;
}


- (void)drawRect:(CGRect)rect 
{
	UIImage *backImage;
	switch (self.entityState) 
	{
		case 1:
			/* Yellow */
			backImage = [UIImage imageNamed:@"LTTableViewCellBack-Yellow"];
			break;
		case 2:
			/* Orange */
			backImage = [UIImage imageNamed:@"LTTableViewCellBack-Orange"];
			break;
		case 3:
			/* Red */
			backImage = [UIImage imageNamed:@"LTTableViewCellBack-Red"];
			break;
		default:
			/* Grey */
			backImage = [UIImage imageNamed:@"LTTableViewCellBack-Gray"];
			break;
	}
	[backImage drawInRect:self.bounds];
	
	// Indent offset 
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
//	UIImage *image = [UIImage imageNamed:@"CellLowerGradient.png"];
//	[image drawInRect:CGRectMake(CGRectGetMinX(self.bounds)+indentOffset, CGRectGetMaxY(self.bounds)-6.0, 
//								 CGRectGetWidth(self.bounds)-indentOffset, 6.0)
//			blendMode:kCGBlendModeSourceAtop 
//				alpha:0.2];
	
	// Over the top gradient 
	
}


- (void)dealloc {
    [super dealloc];
}

@end
