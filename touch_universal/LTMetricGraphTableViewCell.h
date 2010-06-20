//
//  LTMetricGraphTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTMetricGraphView.h"

@interface LTMetricGraphTableViewCell : UITableViewCell 
{
	LTMetricGraphView *graphView;
	UIScrollView *scrollView;
}

@property (retain) LTMetricGraphView *graphView;

@end
