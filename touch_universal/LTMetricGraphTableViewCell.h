//
//  LTMetricGraphTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTGraphView.h"

@class LTGraphView, LTGraphBackgroundView;

@interface LTMetricGraphTableViewCell : UITableViewCell 
{
	LTGraphView *graphView;
	LTGraphBackgroundView *graphBackground;
	UIScrollView *scrollView;
	
	UILabel *leftMinLabel;
	UILabel *leftAvgLabel;
	UILabel *leftMaxLabel;
	UILabel *rightMinLabel;
	UILabel *rightAvgLabel;
	UILabel *rightMaxLabel;
	
}

@property (nonatomic,retain) LTGraphView *graphView;

@end
