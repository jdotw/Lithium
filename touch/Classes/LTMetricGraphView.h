//
//  UIMetricGraphView.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTMetricGraphRequest.h"
#import "LTEntity.h"

@interface LTMetricGraphView : UIView 
{
	LTMetricGraphRequest *graphRequest;
	LTEntity *metric;

	UIActivityIndicatorView *spinner;
	UILabel *notRecordedLabel;
	UIImageView *imageView;
	UIImageView *reflectionView;
	
	BOOL graphRequestNeedsRefresh;
	
	UILabel *leftDateLabel;
	UILabel *rightDateLabel;
	CGFloat dateLabelFontSize;
	CGFloat yLabelFontSize;
	UILabel *minLabel;
	UILabel *avgLabel;
	UILabel *maxLabel;
	UILabel *scaledMinLabel;
	UILabel *scaledAvgLabel;
	UILabel *scaledMaxLabel;	
	UILabel *deviceLabel;
	UILabel *metricLabel;
	
	BOOL dateLabelsHidden;

	BOOL useArtificialScale;
	float scaledMax;
	float scaledMin;
	BOOL animateScaling;
	int graphViewStyle;			/* 1=Small 2=Medium 3=Large */
}

@property (retain) LTMetricGraphRequest *graphRequest;
@property (retain) LTEntity *metric;
@property (assign) BOOL dateLabelsHidden;

@property (assign) BOOL useArtificialScale;
@property (assign) float scaledMax;
@property (assign) float scaledMin;
@property (assign) BOOL animateScaling;
@property (assign) int graphViewStyle;
- (UIImage *)reflectedImage:(UIImageView *)fromImage withHeight:(NSUInteger)height;

@property (readonly) CGRect graphArea;
@property (readonly) CGRect graphImageArea;

@end
