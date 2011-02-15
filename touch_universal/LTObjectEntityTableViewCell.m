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
    [_valueEntity release];
    [super dealloc];
}

- (void) valueEntityValueChanged:(NSNotification *)note
{
    self.detailTextLabel.text = _valueEntity.currentValue;
    [self setNeedsLayout];
}

- (void) entityChildrenChanged:(NSNotification *)note
{
    if (self.entity.valueMetric != _valueEntity)
    {
        /* Value Metric has changed */
        if (_valueEntity)
        {
            /* Remove old */
            [[NSNotificationCenter defaultCenter] removeObserver:self
                                                            name:kLTEntityValueChanged
                                                          object:_valueEntity];
            [_valueEntity release];
            _valueEntity = nil;
        }
        
        /* Add new */
        if (self.entity.valueMetric)
        {
            _valueEntity = [self.entity.valueMetric retain];
            
            /* Add and fire value changed notification */
            [self valueEntityValueChanged:nil];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(valueEntityValueChanged:)
                                                         name:kLTEntityValueChanged
                                                       object:_valueEntity];
        }
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
    if (_valueEntity)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self
                                                        name:kLTEntityValueChanged
                                                      object:_valueEntity];
        [_valueEntity release];
        _valueEntity = nil;
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
