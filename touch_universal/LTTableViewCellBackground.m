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

@synthesize entityState, cell, drawEntityStateBackgroundColor;

- (void) setEntityState:(int)value
{
	entityState = value;
	
	/* Ensure the cell is re-drawn with the right background */
	[self setNeedsDisplay];
}

- (id)initWithFrame:(CGRect)frame 
{
    if (self = [super initWithFrame:frame]) 
	{
		self.drawEntityStateBackgroundColor = YES;
    }
    return self;
}

- (void)drawRect:(CGRect)rect 
{
	/* Draw status-color background */
	UIImage *backImage;
	if (drawEntityStateBackgroundColor)
	{
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
	}
	else 
	{
		backImage = [UIImage imageNamed:@"LTTableViewCellBack-Gray"];
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
}

- (void)dealloc {
    [super dealloc];
}

@end
