//
//  LTMetricTableViewCell.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LTMetricTableViewCell.h"
#import "LTEntity.h"
#import "LTEntityDescriptor.h"

@implementation LTMetricTableViewCell

- (id)initWithReuseIdentifier:(NSString *)reuseIdentifier
{
    if (self = [super initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:reuseIdentifier]) 
	{
		self.detailTextLabel.font = [UIFont systemFontOfSize:11.0];
    }
    return self;
}

- (void)layoutSubviews 
{
    [super layoutSubviews];
	
//	if ([subtitleLabel.text length] > 0)
//	{
//		CGRect textLabelRect = self.textLabel.frame;
//		textLabelRect.origin.y -= subtitleLabel.font.pointSize * 0.8;
//		self.textLabel.frame = CGRectIntegral(textLabelRect);
//		subtitleLabel.frame = CGRectMake(CGRectGetMinX(self.textLabel.frame), 
//										 CGRectGetMaxY(self.textLabel.frame) + 2.0,
//										 CGRectGetWidth(self.textLabel.frame), subtitleLabel.font.pointSize);
//	}
//	else subtitleLabel.frame = CGRectZero;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
	/*
	 Views are drawn most efficiently when they are opaque and do not have a clear background, so in newLabelForMainText: the labels are made opaque and given a white background.  To show selection properly, however, the views need to be transparent (so that the selection color shows through).  
     */
	[super setSelected:selected animated:animated];	
}

- (void)dealloc 
{
	[valueLabel release];
    [super dealloc];
}

- (void) setText:(NSString *)value
{ return; }

- (void) setEntity:(LTEntity *)value
{
	if (value.type == 5 && value.children.count == 1)
	{
		value = [value.children objectAtIndex:0];
	}
	[super setEntity:value];
	
	NSString *desc = self.entity.desc;
	if (![self.entity.entityDescriptor.objName isEqualToString:@"master"]) 
	{ desc = [NSString stringWithFormat:@"%@ %@", self.entity.entityDescriptor.objName, desc]; }
 	self.textLabel.text = desc;
	
	if (showCurrentValue)
	{
		valueLabel = [[UILabel alloc] initWithFrame:CGRectZero];
		valueLabel.backgroundColor = [UIColor clearColor];
		valueLabel.textColor = [UIColor whiteColor];
		valueLabel.highlightedTextColor = [UIColor whiteColor];
		valueLabel.opaque = NO;
		valueLabel.font = [UIFont systemFontOfSize:16.0];
		valueLabel.textAlignment = UITextAlignmentRight;
		self.accessoryView = valueLabel;
		[valueLabel release];
		
		valueLabel.text = self.entity.currentValue;
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bits" withString:@"b"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bytes" withString:@"B"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"bit" withString:@"b"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"byte" withString:@"B"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"writes" withString:@"wr"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"reads" withString:@"rd"];
		valueLabel.text = [valueLabel.text stringByReplacingOccurrencesOfString:@"sec" withString:@"s"];
		
		CGSize stringSize = [valueLabel.text sizeWithFont:valueLabel.font];
		valueLabel.frame = CGRectMake(0., 0., stringSize.width, stringSize.height);
	}
	else 
	{
		self.accessoryView = nil;
		valueLabel = nil;
	}
	if (showFullLocation)
	{
		NSMutableString *location = [NSMutableString stringWithFormat:@"%@ %@", self.entity.entityDescriptor.devDesc, self.entity.entityDescriptor.cntDesc];
		if (![self.entity.entityDescriptor.siteName isEqualToString:@"default"]) [location appendFormat:@" @ %@", self.entity.entityDescriptor.siteDesc];
		
		self.detailTextLabel.text = location;
	}
}

@synthesize showFullLocation;
@synthesize showCurrentValue;

@end
