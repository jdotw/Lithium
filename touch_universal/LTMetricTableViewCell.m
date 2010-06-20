//
//  LTMetricTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricTableViewCell.h"


@implementation LTMetricTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:style reuseIdentifier:reuseIdentifier]) 
	{
		self.textLabel.hidden = YES;
        
        self.metricLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:16.0 bold:YES]; 
		self.metricLabel.textAlignment = UITextAlignmentLeft;
		self.metricLabel.text = @"";
		[self.contentView addSubview:self.metricLabel];
		[self.metricLabel release];

        self.currentValueLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor lightGrayColor] fontSize:15.0 bold:NO];
		self.currentValueLabel.textAlignment = UITextAlignmentRight;
		self.currentValueLabel.text = @"---";
		[self.contentView addSubview:self.currentValueLabel];
		[self.currentValueLabel release];
		
		self.deviceLabel = [self newLabelWithPrimaryColor:[UIColor blackColor] selectedColor:[UIColor whiteColor] fontSize:12.0 bold:NO]; 
		self.deviceLabel.textAlignment = UITextAlignmentLeft;
		self.deviceLabel.text = @"Device Name";
		[self.contentView addSubview:self.deviceLabel];
		[self.deviceLabel release];		
    }
    return self;
}

- (void)layoutSubviews 
{
    [super layoutSubviews];
			
	CGFloat xOffset = CGRectGetMinX(self.contentView.bounds) + ([self indentationLevel] * [self indentationWidth]) + 8.0;
	if (self.imageView.image)
	{ xOffset  = xOffset + 36.0; }
	CGFloat contentWidth = CGRectGetWidth(self.contentView.bounds) - xOffset;
	CGRect contentRect = CGRectMake(xOffset, CGRectGetMinY(self.contentView.bounds),
									 contentWidth, CGRectGetHeight(self.contentView.bounds));
	
	CGFloat leftColWidth;
	if (showCurrentValue) leftColWidth = contentWidth - 80.0;
	else leftColWidth = contentWidth - 42.0;
	
	// Place the name labels.
		CGRect frame;
	if (showFullLocation)
	{
		frame = CGRectMake(xOffset, roundf(CGRectGetMidY(contentRect) - self.metricLabel.font.pointSize + 1.0), 
						   leftColWidth, self.metricLabel.font.pointSize + 2.0);
		self.metricLabel.frame = frame;
		
		frame = CGRectMake(xOffset, roundf(CGRectGetMidY(contentRect)+2.0), 
						   leftColWidth, self.deviceLabel.font.pointSize + 2.0);
		self.deviceLabel.frame = frame;
		
		self.deviceLabel.hidden = NO;
	}
	else
	{
		frame = CGRectMake(xOffset, roundf(CGRectGetMidY(contentRect) - ((self.metricLabel.font.pointSize+2) * 0.5)), 
						   leftColWidth, self.metricLabel.font.pointSize+2.0);
		self.metricLabel.frame = frame;
		frame = CGRectZero;
		self.deviceLabel.frame = frame;
		self.deviceLabel.hidden = YES;
	}
	
	// Place the value label 
	CGFloat rightOffset = leftColWidth + 8.0;
	frame = CGRectMake(xOffset + rightOffset, 5.0, contentWidth - rightOffset - 4, 32);
	self.currentValueLabel.frame = frame;
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

- (UILabel *)newLabelWithPrimaryColor:(UIColor *)primaryColor selectedColor:(UIColor *)selectedColor fontSize:(CGFloat)fontSize bold:(BOOL)bold
{
	/*
	 Create and configure a label.
	 */
	
    UIFont *font;
    if (bold) 
	{
        font = [UIFont boldSystemFontOfSize:fontSize];
    } else 
	{
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    /*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so set these defaults.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  This is handled in setSelected:animated:.
	 */
	UILabel *newLabel = [[UILabel alloc] initWithFrame:CGRectZero];
	newLabel.backgroundColor = [UIColor clearColor];
	newLabel.opaque = YES;
	newLabel.textColor = primaryColor;
	newLabel.highlightedTextColor = selectedColor;
	newLabel.font = font;
	newLabel.textColor = [UIColor whiteColor];
	newLabel.highlightedTextColor = [UIColor whiteColor];
	newLabel.shadowColor = [UIColor blackColor];
	newLabel.shadowOffset = CGSizeMake(0.0, -1.0);		
	
	return newLabel;
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
	metricLabel.text = entity.desc;
	currentValueLabel.text = entity.currentValue;
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
	currentValueLabel.text = [currentValueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
	if (entity.type == 6) self.showCurrentValue = YES;
	else self.showCurrentValue = NO;
}

@synthesize showFullLocation;
@synthesize showCurrentValue;

@end
