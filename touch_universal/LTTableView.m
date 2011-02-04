//
//  LTTableView.m
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "LTTableView.h"

#define SHADOW_HEIGHT 30
#define SHADOW_INVERSE_OFFSET 15;

@implementation LTTableView

@synthesize backgroundImage;

+ (UITableViewCellStyle) defaultCellStyle
{
	return UITableViewStylePlain;
}

- (void) awakeFromNib
{
	self.separatorStyle = UITableViewCellSeparatorStyleNone;
	self.backgroundImage = [UIImage imageNamed:@"noiseback-light.png"];
}

- (void) drawRect:(CGRect)theRect
{
	[super drawRect:theRect];
	if (self.backgroundImage)
	{
		[self.backgroundImage drawAsPatternInRect:self.bounds];
	}
}

- (void) setBackgroundColor:(UIColor *)color
{
	[super setBackgroundColor:color];
	self.backgroundImage = nil;
}

@end
