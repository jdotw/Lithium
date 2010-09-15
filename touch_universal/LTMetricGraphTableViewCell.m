//
//  LTMetricGraphTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricGraphTableViewCell.h"
#import "LTMetricTableViewCellBackground.h"

@implementation LTMetricGraphTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
		self.backgroundView = [[[LTMetricTableViewCellBackground alloc] initWithFrame:CGRectZero] autorelease];
        self.graphView = [[LTGraphView alloc] initWithFrame:CGRectZero];
//		self.graphView.graphViewStyle = 2;
		[self.contentView addSubview:self.graphView];
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
    CGRect contentRect = self.contentView.bounds;
	graphView.frame = contentRect;
}

- (void)dealloc 
{
	[graphView release];
    [super dealloc];
}

@synthesize graphView;


@end
