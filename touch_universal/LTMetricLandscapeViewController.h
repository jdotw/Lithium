//
//  LTMetricLandscapeViewController.h
//  Lithium
//
//  Created by James Wilson on 7/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"

@class LTGraphView;

@interface LTMetricLandscapeViewController : UIViewController <UIScrollViewDelegate>
{
	IBOutlet UIScrollView *graphScrollView;
	IBOutlet UILabel *leftMaxLabel;
	IBOutlet UILabel *leftAvgLabel;
	IBOutlet UILabel *leftMinLabel;
	IBOutlet UILabel *rightMaxLabel;
	IBOutlet UILabel *rightAvgLabel;
	IBOutlet UILabel *rightMinLabel;
    IBOutlet UILabel *metricLabel;
	
	LTGraphView *graphView;
}

@property (retain,nonatomic) LTEntity *metric;

@end
