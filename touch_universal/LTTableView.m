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

+ (UITableViewCellStyle) defaultCellStyle
{
	return UITableViewStylePlain;
}

- (void) awakeFromNib
{
	self.separatorStyle = UITableViewCellSeparatorStyleNone;
}

- (void) drawRect:(CGRect)theRect
{
	UIImage *image = [UIImage imageNamed:@"noiseback-light.png"];
	[image drawInRect:self.bounds blendMode:kCGBlendModeSourceAtop alpha:1.0];
	NSLog (@"Bounds is %@", NSStringFromCGRect(self.bounds));
}

@end
