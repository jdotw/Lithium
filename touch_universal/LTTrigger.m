//
//  LTTrigger.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTrigger.h"


@implementation LTTrigger

@synthesize valueType, triggerType, effect, xValue, yValue, duration;

- (NSString *) conditionString
{
    switch(triggerType)
    {
        case TRGTYPE_LT:
            return [NSString stringWithFormat:@"< %@", self.xValue];
        case TRGTYPE_GT:
            return [NSString stringWithFormat:@"> %@", self.xValue];
        case TRGTYPE_RANGE:
            return [NSString stringWithFormat:@"%@ - %@", self.xValue, self.yValue];
        case TRGTYPE_NOTEQUAL:
            return [NSString stringWithFormat:@"!= %@", self.xValue];
        default:
            return self.xValue;
    }
}

@end
