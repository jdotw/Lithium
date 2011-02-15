//
//  LTMetricEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 15/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTMetricEntityTableViewCell.h"
#import "LTEntity.h"

@implementation LTMetricEntityTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:reuseIdentifier];
    if (!self) return nil;
    
    return self;
}

- (void) entityValueChanged:(NSNotification *)notification
{
    self.detailTextLabel.text = self.entity.currentValue;
    [self setNeedsLayout];
}

- (void) setEntity:(LTEntity *)value
{
    if (self.entity)
    {
        /* Remove value observer */
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityValueChanged 
                                                      object:self.entity];
    }
    
    /* Call super to set property */
    [super setEntity:value];
    
    if (self.entity)
    {
        /* Add and fire value observer */
        [self entityValueChanged:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(entityValueChanged:)
                                                     name:kLTEntityValueChanged
                                                   object:self.entity];
    }
}

@end
