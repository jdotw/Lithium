//
//  LTMetricGraphTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTGraphView.h"

@class LTGraphView;

@interface LTMetricGraphTableViewCell : UITableViewCell 
{
	LTGraphView *graphView;
	UIScrollView *scrollView;
	
	UILabel *leftMinLabel;
	UILabel *leftAvgLabel;
	UILabel *leftMaxLabel;
	UILabel *rightMinLabel;
	UILabel *rightAvgLabel;
	UILabel *rightMaxLabel;
	
}

@property (retain) LTGraphView *graphView;

@end
