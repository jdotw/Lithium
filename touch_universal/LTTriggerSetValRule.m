//
//  LTTriggerSetValRule.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTriggerSetValRule.h"


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

@end
