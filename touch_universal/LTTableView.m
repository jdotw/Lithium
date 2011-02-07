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
    self.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"noiseback-light.png"]];
}

@end
