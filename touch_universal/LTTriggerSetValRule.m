//
//  LTTriggerSetValRule.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetValRule.h"
#import "LTTrigger.h"

@implementation LTTriggerSetValRule

@synthesize identifier, siteName, siteDesc;
@synthesize devName, devDesc, objName, objDesc;
@synthesize trgName, trgDesc, duration, triggerType;
@synthesize adminState, xValue, yValue;

- (LTTriggerSetValRule *) moreSpecificRule:(LTTriggerSetValRule *)otherRule
{
    /* Returns the more specific of the two rules */
    if (self.objName && !otherRule.objName) return self;
    else if (!self.objName && otherRule.objName) return otherRule;
    
    if (self.devName && !otherRule.devName) return self;
    else if (!self.devName && otherRule.devName) return otherRule;

    if (self.siteName && !otherRule.siteName) return self;
    else if (!self.siteName && otherRule.siteName) return otherRule;
    
    return self;    // Tied
}

- (BOOL) hasTheSameEffectAs:(LTTriggerSetValRule *)matchRule
{
    /* Returns true if the trigger type, values and duration match */
    if (self.triggerType == matchRule.triggerType && self.duration == matchRule.duration && self.adminState == matchRule.adminState)
    {
        /* Type and duration matches, confirm the values match */
        if ([self.xValue isEqualToString:matchRule.xValue] 
            && (self.triggerType != TRGTYPE_RANGE || [self.yValue isEqualToString:matchRule.yValue]))
        {
            /* String values match */
            return YES;
        }
        else if ([self.xValue floatValue] == [matchRule.xValue floatValue] 
                 && (self.triggerType != TRGTYPE_RANGE || [self.yValue floatValue] == [matchRule.yValue floatValue]))
        {
            /* Float values match */
            return YES;
        }
        else 
        {
            /* Values dont match */
            return NO;
        }
    }
    else return NO; // Type or duration don't match
}

@end
