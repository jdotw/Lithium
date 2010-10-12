//
//  LTIconTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 12/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LTIconTableViewCell.h"


@implementation LTIconTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier 
{
    if ((self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])) {
        // Initialization code
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated 
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

+ (int) itemsPerRowAtHeight:(CGFloat)height
{
	return height / 4;
}

- (void)dealloc {
   [super dealloc];
}

- (void) layoutSubviews
{
	/* Lays out views equidistant horizontally */
	CGFloat viewWidth = self.bounds.size.width / [[self superclass] itemsPerRowAtHeight:self.bounds.size.height];	
	for (UIView *view in [self subviews])
	{
		view.frame = CGRectMake([[self subviews] indexOfObject:view] * viewWidth, CGRectGetMinY(self.bounds), viewWidth, CGRectGetHeight(self.bounds));
	}
}

- (void) removeAllSubviews
{
	for (UIView *view in [self subviews])
	{
		[view removeFromSuperview];
	}
}

@end
