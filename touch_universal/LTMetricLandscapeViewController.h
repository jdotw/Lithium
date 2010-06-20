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

@interface LTMetricLandscapeViewController : UIViewController 
{
	LTEntity *metric;
	IBOutlet UIScrollView *scrollView;

	NSMutableArray *graphViews;
	LTMetricGraphView *visibleGraphView;
	
	LTMetricGraphRequest *cachedGraphRequest;
}

@property (retain) LTEntity *metric;
@property (retain) LTMetricGraphRequest *cachedGraphRequest;


@end
