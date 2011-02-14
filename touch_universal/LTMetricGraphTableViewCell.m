//
//  LTMetricGraphTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricGraphTableViewCell.h"
#import "LTMetricGraphTableViewCellBackground.h"
#import "LTGraphBackgroundView.h"

@implementation LTMetricGraphTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if ((self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])) 
	{
		self.backgroundView = [[[LTMetricGraphTableViewCellBackground alloc] initWithFrame:CGRectZero] autorelease];
        self.graphView = [[[LTGraphView alloc] initWithFrame:CGRectZero] autorelease];
		graphBackground = [[[LTGraphBackgroundView alloc] initWithFrame:CGRectZero] autorelease];
		[self.contentView addSubview:graphBackground];
		[self.contentView insertSubview:self.graphView aboveSubview:graphBackground];
		graphBackground.opaque = NO;
		
		/* Create Labels */
		leftMinLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		leftMinLabel.textColor = [UIColor colorWithWhite:1.0 alpha:0.6];
		leftMinLabel.backgroundColor = [UIColor clearColor];
		leftMinLabel.font = [UIFont systemFontOfSize:10.0];
		leftMinLabel.text = @"Min";
		leftMinLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		leftMinLabel.layer.shadowRadius = 3.0;
		leftMinLabel.layer.shadowOpacity = 0.8;
		[self.contentView addSubview:leftMinLabel];
		[self.graphView.minLabels addObject:leftMinLabel];
		leftAvgLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		leftAvgLabel.textColor = leftMinLabel.textColor;
		leftAvgLabel.backgroundColor = leftMinLabel.backgroundColor;
		leftAvgLabel.font = leftMinLabel.font;
		leftAvgLabel.text = @"Avg";
		leftAvgLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		leftAvgLabel.layer.shadowRadius = 3.0;
		leftAvgLabel.layer.shadowOpacity = 0.8;
		[self.contentView addSubview:leftAvgLabel];
		[self.graphView.avgLabels addObject:leftAvgLabel];
		leftMaxLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		leftMaxLabel.textColor = leftMinLabel.textColor;
		leftMaxLabel.backgroundColor = leftMinLabel.backgroundColor;
		leftMaxLabel.font = leftMinLabel.font;
		leftMaxLabel.text = @"Min";
		leftMaxLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		leftMaxLabel.layer.shadowRadius = 3.0;
		leftMaxLabel.layer.shadowOpacity = 0.8;
		[self.contentView addSubview:leftMaxLabel];
		[self.graphView.maxLabels addObject:leftMaxLabel];
		rightMinLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		rightMinLabel.textColor = leftMinLabel.textColor;
		rightMinLabel.backgroundColor = leftMinLabel.backgroundColor;
		rightMinLabel.font = leftMinLabel.font;
		rightMinLabel.text = @"Min";
		rightMinLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		rightMinLabel.layer.shadowRadius = 3.0;
		rightMinLabel.layer.shadowOpacity = 0.8;
		rightMinLabel.textAlignment = UITextAlignmentRight;
		[self.contentView addSubview:rightMinLabel];
		[self.graphView.minLabels addObject:rightMinLabel];
		rightAvgLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		rightAvgLabel.textColor = leftMinLabel.textColor;
		rightAvgLabel.backgroundColor = leftMinLabel.backgroundColor;
		rightAvgLabel.font = leftMinLabel.font;
		rightAvgLabel.text = @"Avg";
		rightAvgLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		rightAvgLabel.layer.shadowRadius = 3.0;
		rightAvgLabel.layer.shadowOpacity = 0.8;
		rightAvgLabel.textAlignment = UITextAlignmentRight;
		[self.contentView addSubview:rightAvgLabel];
		[self.graphView.avgLabels addObject:rightAvgLabel];
		rightMaxLabel = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		rightMaxLabel.textColor = leftMinLabel.textColor;
		rightMaxLabel.backgroundColor = leftMinLabel.backgroundColor;
		rightMaxLabel.font = leftMinLabel.font;
		rightMaxLabel.text = @"Min";
		rightMaxLabel.layer.shadowOffset = CGSizeMake(2.0, 2.0);
		rightMaxLabel.layer.shadowRadius = 3.0;
		rightMaxLabel.layer.shadowOpacity = 0.8;
		rightMaxLabel.textAlignment = UITextAlignmentRight;
		[self.contentView addSubview:rightMaxLabel];
		[self.graphView.maxLabels addObject:rightMaxLabel];	
		
		graphBackground.minLabel = leftMinLabel;
		graphBackground.avgLabel = leftAvgLabel;
		graphBackground.maxLabel = leftMaxLabel;
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];
}

- (void)layoutSubviews 
{
    [super layoutSubviews];
	
	/* Label Layout */
	CGFloat labelYPadding = 2.0;
	CGFloat labelXPadding = 2.0;
	CGFloat labelHeight = 11.0;
	CGFloat labelWidth = CGRectGetWidth(self.contentView.bounds) * 0.5;
	leftMaxLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMinX(self.contentView.bounds) + labelXPadding, CGRectGetMinY(self.contentView.bounds) + labelYPadding,
									labelWidth-(labelXPadding*2.0), labelHeight));
	leftAvgLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMinX(self.contentView.bounds) + labelXPadding, (CGRectGetMidY(self.contentView.bounds) - (labelHeight*0.5))+ labelYPadding,
                                                   labelWidth-(labelXPadding*2.0), labelHeight));
	leftMinLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMinX(self.contentView.bounds) + labelXPadding, CGRectGetMaxY(self.contentView.bounds) - labelYPadding - labelHeight,
                                                   labelWidth-(labelXPadding*2.0), labelHeight));
	rightMaxLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMaxX(self.contentView.bounds) - labelWidth - labelXPadding, CGRectGetMinY(self.contentView.bounds) + labelYPadding,
                                                    labelWidth, labelHeight));
	rightAvgLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMaxX(self.contentView.bounds) - labelWidth - labelXPadding, (CGRectGetMidY(self.contentView.bounds) - (labelHeight*0.5))+ labelYPadding, labelWidth, labelHeight));
	rightMinLabel.frame = CGRectIntegral(CGRectMake(CGRectGetMaxX(self.contentView.bounds) - labelWidth - labelXPadding, CGRectGetMaxY(self.contentView.bounds) - labelYPadding - labelHeight, labelWidth, labelHeight));
	
	/* Background layout */
	graphBackground.frame = self.contentView.bounds;

	/* Graph Layout */
	graphView.frame = CGRectIntegral(CGRectMake(CGRectGetMinX(self.contentView.bounds), CGRectGetMinY(graphBackground.maxLineRect)+1,
                                                CGRectGetWidth(self.contentView.bounds),  (CGRectGetMinY(graphBackground.minLineRect)+1) - CGRectGetMinY(graphBackground.maxLineRect)));	
}

- (void)dealloc 
{
	[graphView release];
    [super dealloc];
}

@synthesize graphView;


@end
