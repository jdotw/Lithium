//
//  LTTrigger.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTTrigger.h"
#import "LTTriggerSetValRule.h"

@implementation LTTrigger

@synthesize triggerType=_triggerType, xValue=_xValue, yValue=_yValue, duration=_duration;
@synthesize valueType, effect, valRules, valRuleDict;

- (NSString *) conditionString
{
    switch(self.triggerType)
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

- (id) init
{
    self = [super init];
    if (!self) return nil;
    
    valRules = [[NSMutableArray array] retain];
    valRuleDict = [[NSMutableDictionary dictionary] retain];
    
    return self;
}

- (void) dealloc
{
    [valRules release];
    [valRuleDict release];
    [super dealloc];
}

- (void) beginAPIUpdate
{
    _apiUpdate = YES;
}

- (void) endAPIUpdate
{
    _apiUpdate = NO;
}

- (void) setTriggerType:(int)triggerType
{
    _triggerType = triggerType;    
    if (_apiUpdate) _apiTriggerType = triggerType;
}

- (void) setXValue:(NSString *)xValue
{
    [_xValue release];
    _xValue = [xValue retain];
    if (_apiUpdate)
    {
        [_apiXValue release];
        _apiXValue = [xValue retain];
    }
}

- (void) setYValue:(NSString *)yValue
{
    [_yValue release];
    _yValue = [yValue retain];
    if (_apiUpdate)
    {
        [_apiYValue release];
        _apiYValue = [yValue retain];
    }
}

- (void) setDuration:(int)duration
{
    _duration = duration;
    if (_apiUpdate) _apiDuration = duration;
}

- (void) setAdminState:(int)value
{
    [super setAdminState:value];
    if (_apiUpdate) _apiAdminState = value;
}

#pragma - Rule Updating

- (LTTriggerSetValRule *) _scopedValRuleForObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Return a ValRule the first appRule that matches the scope */
    for (LTTriggerSetValRule *valRule in self.valRules)
    {
        if ((objName && valRule.objName) && [valRule.objName isEqualToString:objName] 
            && (devName && valRule.devName) && [valRule.devName isEqualToString:devName] 
            && (siteName && valRule.siteName) && [valRule.siteName isEqualToString:siteName])
        {
            /* Match! */
            return valRule;
        }
    }
    return nil; // No match
}

- (BOOL) triggerHasChanged
{
    if (_triggerType != _apiTriggerType
        || ![_xValue isEqualToString:_apiXValue]
        || (_triggerType == TRGTYPE_RANGE && ![_yValue isEqualToString:_apiYValue])
        || _duration != _apiDuration
        || self.adminState != _apiAdminState)
    { return YES; }
    else 
    { return NO; }
}

- (LTTriggerSetValRule *) ruleToUpdateForChangesInObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Returns an array of the ValRule objects that need
     * to be pushed to LithiumCore in order to affect any changes made
     * in the trigger based on the scope of the 
     * changes being dictated by the supplied objName, devName and siteName
     */
    
    LTTriggerSetValRule *valRule = nil;

    /* Determine local appRule changes */
    if (self.triggerHasChanged)
    {
        valRule = [self _scopedValRuleForObject:objName device:devName site:siteName];
        if (!valRule) 
        {
            valRule = [[LTTriggerSetValRule new] autorelease];
            valRule.objName = objName;
            if (objName) valRule.objDesc = self.metric.object.desc;
            valRule.devName = devName;
            if (devName) valRule.devDesc = self.metric.device.desc;
            valRule.siteName = siteName;
            if (siteName) valRule.siteDesc = self.metric.site.desc;
            valRule.trgName = self.name;
            valRule.trgDesc = self.desc;
        }
        valRule.adminState = self.adminState;
        valRule.xValue = self.xValue;
        if (self.triggerType == TRGTYPE_RANGE) valRule.yValue = self.yValue;
        else valRule.yValue = nil;
        valRule.duration = self.duration;
        valRule.triggerType = self.triggerType;
    }
    
    return valRule;
}

@end
