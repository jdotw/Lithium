//
//  LTSummaryTableView.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTSummaryTableView.h"


@implementation LTSummaryTableView

- (void) drawRect:(CGRect)rect
{
	[[UIColor blackColor] setFill];
	UIRectFill(self.bounds);
	[[UIImage imageNamed:@"trabitine.jpg"] drawInRect:self.bounds blendMode:kCGBlendModeNormal alpha:0.3];
}

@end
