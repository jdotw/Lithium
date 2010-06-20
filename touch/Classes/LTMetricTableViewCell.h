//
//  LTMetricTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntityTableViewCell.h"

@interface LTMetricTableViewCell : LTEntityTableViewCell 
{
    UILabel *metricLabel;
	UILabel *deviceLabel;
    UILabel *currentValueLabel;
	
	BOOL showFullLocation;
	BOOL showCurrentValue;
}

@property (retain) UILabel *metricLabel;
@property (retain) UILabel *deviceLabel;
@property (retain) UILabel *currentValueLabel;
@property (assign) BOOL showFullLocation;
@property (assign) BOOL showCurrentValue;

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold;

@end
