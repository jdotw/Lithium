//
//  LTContainerIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTEntityIconViewController.h"

@class LTEntity, LTMetricGraphRequest, LTContainerIconViewGraphView;

@interface LTContainerIconViewController : LTEntityIconViewController 
{
	LTMetricGraphRequest *graphReq;
	IBOutlet LTContainerIconViewGraphView *graphView;
}

- (id) initWithContainer:(LTEntity *)container;

@end
