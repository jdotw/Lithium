//
//  LTGraphLegendEntityView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTGraphLegendEntityView : UIView 
{
	UIView *swatchView;
	UILabel *descLabel;
	UILabel *valueLabel;
	
	UIColor *swatchColor;
	LTEntity *entity;
}

@property (nonatomic,retain) UIColor *swatchColor;
@property (nonatomic,retain) LTEntity *entity;

- (void) presentPopoverForEntityFromRect:(CGRect)rect;

@end
