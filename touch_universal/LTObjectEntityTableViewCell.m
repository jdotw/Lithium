//
//  LTObjectEntityTableViewCell.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 15/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTObjectEntityTableViewCell.h"
#import "LTEntity.h"

@implementation LTObjectEntityTableViewCell

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:reuseIdentifier];
    if (!self) return nil;
    
    return self;
}

- (void) dealloc
{
    /* Remove value metric observers */
    for (LTEntity *valueMetric in _valueMetrics)
    {
        /* Remove old */
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityValueChanged
                                                      object:valueMetric];
    }
    
    /* Release ivars */
    [_valueMetrics release];
    
    [super dealloc];
}

- (void) valueEntityValueChanged:(NSNotification *)note
{
    if (_valueMetrics.count > 0)
    {
        NSSortDescriptor *sortDesc = [NSSortDescriptor sortDescriptorWithKey:@"currentFloatValue" ascending:NO];
        NSArray *sortedValueMetrics = [_valueMetrics sortedArrayUsingDescriptors:[NSArray arrayWithObject:sortDesc]];
        self.detailTextLabel.text = [[sortedValueMetrics objectAtIndex:0] currentValue];
    }
    else self.detailTextLabel.text = nil;
    [self setNeedsLayout];
}

- (void) entityChildrenChanged:(NSNotification *)note
{
    /* Remove old valueMetrics observers */
    for (LTEntity *valueMetric in _valueMetrics)
    {
        /* Remove old */
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityValueChanged
                                                      object:valueMetric];
    }
    
    /* Set new value metrics array */
    [_valueMetrics release];
    _valueMetrics = [[self.entity valueMetrics] copy];

    /* Force update of value labels */
    [self valueEntityValueChanged:nil];
    
    /* Add new valueMetrics observers */
    for (LTEntity *valueMetric in _valueMetrics)
    {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(valueEntityValueChanged:)
                                                     name:kLTEntityValueChanged
                                                   object:valueMetric];
    }
    
    /* Check for alias */
    if (self.entity.hasAlias)
    {
        self.showAlias = YES;
    }
}

- (void) setEntity:(LTEntity *)entity
{
    /* Remove old observers */
    if (self.entity)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityChildrenChanged
                                                      object:self.entity];
    }
    
    /* Call super to set property */
    [super setEntity:entity];
    
    /* Add observrs */
    if (self.entity)
    {
        /* Add and fire children observer */
        [self entityChildrenChanged:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(entityChildrenChanged:)
                                                     name:kLTEntityChildrenChanged
                                                   object:self.entity];
    }
}

@end
