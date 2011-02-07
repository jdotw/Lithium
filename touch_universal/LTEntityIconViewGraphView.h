//
//  LTContainerIconViewGraphView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 29/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity, LTMetricGraphRequest;

@interface LTEntityIconViewGraphView : UIView 
{
	LTMetricGraphRequest *graphReq;
}

@property (nonatomic,retain) LTEntity *entity;

@end