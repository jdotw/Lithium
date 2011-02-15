//
//  LTContainerEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 15/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTContainerEntityTableViewCell.h"

#import <QuartzCore/QuartzCore.h>

@implementation LTContainerEntityTableViewCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:reuseIdentifier];
    if (self) {
        // Initialization code
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)dealloc
{
    [super dealloc];
}

@end
