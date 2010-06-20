//
//  LTLeftRightTableCell.h
//  Lithium
//
//  Created by James Wilson on 21/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewCell.h"

@interface LTLeftRightTableCell : LTTableViewCell 
{
    UILabel *leftLabel;
    UILabel *rightLabel;
	
	BOOL showFullLocation;
}

@property (retain) UILabel *leftLabel;
@property (retain) UILabel *rightLabel;

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold;

@end
