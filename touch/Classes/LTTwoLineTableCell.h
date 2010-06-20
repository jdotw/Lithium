//
//  LTTwoLineTableCell.h
//  Lithium
//
//  Created by James Wilson on 1/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewCell.h"

@interface LTTwoLineTableCell : LTTableViewCell 
{
	UILabel *topLineLabel;
	UILabel *bottomLineLabel;
	
	BOOL hasRightLabel;
	UILabel *rightLabel;
	
	UIImage *dotImage;
}

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold;

@property (retain) UILabel *topLineLabel;
@property (retain) UILabel *bottomLineLabel;
@property (assign) BOOL hasRightLabel;
@property (retain) UILabel *rightLabel;
@property (retain) UIImage *dotImage;

@end
