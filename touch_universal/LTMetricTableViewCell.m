//
//  LTMetricTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricTableViewCell.h"


@implementation LTMetricTableViewCell

- (id)initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:reuseIdentifier]) 
	{
		self.currentValueLabel= [[UILabel alloc] initWithFrame:CGRectZero];
		self.currentValueLabel.backgroundColor = [UIColor clearColor];
		self.currentValueLabel.opaque = YES;
		self.currentValueLabel.font = [UIFont boldSystemFontOfSize:16.0];
		self.currentValueLabel.textAlignment = UITextAlignmentRight;
		self.currentValueLabel.text = @"---";
		[self.contentView addSubview:self.currentValueLabel];
		[self.currentValueLabel release];		
    }
    return self;
}

- (void)layoutSubviews 
{
    [super layoutSubviews];
	
	/* Layout the current value label */
			
	CGFloat xOffset = CGRectGetMinX(self.contentView.bounds) + ([self indentationLevel] * [self indentationWidth]) + 8.0;
	if (self.imageView.image)
	{ xOffset  = xOffset + 36.0; }
	CGFloat contentWidth = CGRectGetWidth(self.contentView.bounds) - xOffset;
	
	CGFloat leftColWidth;
	if (showCurrentValue) leftColWidth = contentWidth - 80.0;
	else leftColWidth = contentWidth - 42.0;
	
	CGFloat rightOffset = leftColWidth + 8.0;
	CGRect valueLabelFrame = CGRectMake(xOffset + rightOffset, 5.0, contentWidth - rightOffset - 4, 32);
	self.currentValueLabel.frame = valueLabelFrame;
	
	/* Adjust (shrink) the text labels */
	CGRect textLabelRect = self.textLabel.frame;
	textLabelRect.size.width = textLabelRect.size.width - valueLabelFrame.size.width;
	if (!self.detailTextLabel.hidden)
	{
		CGRect detailTextLabelRect = self.textLabel.frame;
		detailTextLabelRect.size.width = detailTextLabelRect.size.width - valueLabelFrame.size.width;
	}
	
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
	/*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so in newLabelForMainText: the labels are made opaque and given a white background.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  
     */
	[super setSelected:selected animated:animated];
	
	self.metricLabel.highlighted = selected;
	self.metricLabel.opaque = !selected;
	
	self.currentValueLabel.highlighted = selected;
	self.currentValueLabel.opaque = !selected;
}

- (void)dealloc {
    [super dealloc];
}

@synthesize metricLabel;
@synthesize deviceLabel;
@synthesize currentValueLabel;

- (void) setText:(NSString *)value
{ return; }

- (void) setEntity:(LTEntity *)value
{
	[super setEntity:value];
	metricLabel.text = self.entity.desc;
	currentValueLabel.text = self.entity.currentValue;
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
	if (self.entity.type == 6) self.showCurrentValue = YES;
	else self.showCurrentValue = NO;
}

@synthesize showFullLocation;
@synthesize showCurrentValue;

@end
