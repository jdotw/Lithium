//
//  LTSummaryIconView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 14/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTSummaryIconView : UIView 
{
	UILabel *label;
	UIImageView *imageView;
	UIButton *button;
}

@property (nonatomic, retain) LTEntity *entity;

- (id)initWithEntity:(LTEntity *)initEntity;

@end
