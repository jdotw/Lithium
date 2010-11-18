//
//  LTMetricLandscapeViewController.h
//  Lithium
//
//  Created by James Wilson on 7/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTMetricGraphView.h"

@class LTGraphView;

@interface LTMetricLandscapeViewController : UIViewController 
{
	IBOutlet UIScrollView *graphScrollView;
	IBOutlet UILabel *leftMaxLabel;
	IBOutlet UILabel *leftAvgLabel;
	IBOutlet UILabel *leftMinLabel;
	IBOutlet UILabel *rightMaxLabel;
	IBOutlet UILabel *rightAvgLabel;
	IBOutlet UILabel *rightMinLabel;
	
	LTGraphView *graphView;
}

@property (retain,nonatomic) LTEntity *metric;

@end
