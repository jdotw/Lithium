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
@synthesize defaultTriggerType, defaultXValue, defaultYValue, defaultDuration;
@synthesize valueType, effect, valRules, valRuleDict;

- (NSString *) conditionString
{
    if (self.adminState != 0)
    { return @"Disabled"; }
    
    NSString *string = nil;
    switch(self.triggerType)
    {
        case TRGTYPE_LT:
            string = [NSString stringWithFormat:@"< %@", self.xValue];
            break;
        case TRGTYPE_GT:
            string = [NSString stringWithFormat:@"> %@", self.xValue];
            break;
        case TRGTYPE_RANGE:
            string = [NSString stringWithFormat:@"%@ - %@", self.xValue, self.yValue];
            break;
        case TRGTYPE_NOTEQUAL:
            string = [NSString stringWithFormat:@"!= %@", self.xValue];
            break;
        default:
            string = self.xValue;
            break;
    }
    
    if (self.duration > 0) string = [string stringByAppendingFormat:@" for %isec", self.duration];
    
    return string;
}

- (NSString *) defaultConditionString
{
    NSString *string = nil;
    switch(self.defaultTriggerType)
    {
        case TRGTYPE_LT:
            string = [NSString stringWithFormat:@"< %@", self.defaultXValue];
            break;
        case TRGTYPE_GT:
            string = [NSString stringWithFormat:@"> %@", self.defaultXValue];
            break;
        case TRGTYPE_RANGE:
            string = [NSString stringWithFormat:@"%@ - %@", self.defaultXValue, self.defaultYValue];
            break;
        case TRGTYPE_NOTEQUAL:
            string = [NSString stringWithFormat:@"!= %@", self.defaultXValue];
            break;
        default:
            string = self.defaultXValue;
            break;
    }
    
    if (self.defaultDuration > 0) string = [string stringByAppendingFormat:@" for %isec", self.defaultDuration];
    
    return string;    
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

#pragma - Defaults 

- (void) restoreDefaults
{
    self.triggerType = self.defaultTriggerType;
    self.duration = self.defaultDuration;
    self.adminState = 0;
    self.xValue = self.defaultXValue;
    self.yValue = self.defaultYValue;
}

#pragma - Rule Updating

- (LTTriggerSetValRule *) _updatedScopedValRuleForObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    LTTriggerSetValRule *matchingRule = nil;
    for (LTTriggerSetValRule *valRule in self.valRules)
    {
        if ((objName && valRule.objName) && [valRule.objName isEqualToString:objName] 
            && (devName && valRule.devName) && [valRule.devName isEqualToString:devName] 
            && (siteName && valRule.siteName) && [valRule.siteName isEqualToString:siteName])
        {
            /* Match! */
            matchingRule = valRule;
            break;
        }
    }

    /* Create updated rule (matching rule may be nil here) */
    LTTriggerSetValRule *updatedRule = [[LTTriggerSetValRule new] autorelease];
    updatedRule.identifier = matchingRule.identifier;
    updatedRule.objName = objName;
    if (objName) updatedRule.objDesc = self.object.desc;
    updatedRule.devName = devName;
    if (devName) updatedRule.devDesc = self.device.desc;
    updatedRule.siteName = siteName;
    if (siteName) updatedRule.siteDesc = self.site.desc;
    updatedRule.trgName = self.name;
    updatedRule.trgDesc = self.desc;
    
    
    updatedRule.adminState = self.adminState;
    updatedRule.xValue = self.xValue;
    if (self.triggerType == TRGTYPE_RANGE) updatedRule.yValue = self.yValue;
    else updatedRule.yValue = nil;
    updatedRule.duration = self.duration;
    updatedRule.triggerType = self.triggerType;

    return updatedRule;
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
    /* Returns the ValRule objects that need
     * to be pushed to LithiumCore in order to affect any changes made
     * in the trigger based on the scope of the 
     * changes being dictated by the supplied objName, devName and siteName
     */
    
    LTTriggerSetValRule *valRule = nil;

    /* Determine local valRule changes required */
    if (self.triggerHasChanged)
    {
        /* Get the updates valrule */
        valRule = [self _updatedScopedValRuleForObject:objName device:devName site:siteName];

        /* Check to see if a less-specific rule or the 
         * default value will have the same affect 
         */
        if ([self lessSpecificRuleMatches:valRule])
        {
            /* There is a less-specific rule in the list, 
             * or the triggers default will have the same affect,
             * return nil here because no rule needs to be 
             * updated, rather it needs to be deleted
             */
            return nil;
        }
        
    }
    
    return valRule;
}

- (LTTriggerSetValRule *) ruleToDeleteForChangesInObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName
{
    /* Returns the rule object that needs to be deleted
     * to affect the current changes within the specific scope
     */
    
    LTTriggerSetValRule *valRule = nil;
    
    /* Determine local valRule changes required */
    if (self.triggerHasChanged)
    {
        /* Get the updates valrule */
        valRule = [self _updatedScopedValRuleForObject:objName device:devName site:siteName];
        
        /* Check to see if a less-specific rule or the 
         * default value will have the same affect 
         */
        if (valRule.identifier > 0 && [self lessSpecificRuleMatches:valRule])
        {
            /* There is a less-specific rule in the list, 
             * or the triggers default will have the same affect,
             * return the valRule here because it should be deleted
             * which will allow the less specific rule or default to
             * take affect.
             */
            return valRule;
        }
        else
        {
            /* The rule has no less-specific or default match,
             * return nil to make sure it is not deleted and allows
             * the rule to be added to the list of rules to be updated
             */
            return nil;
        }
        
    }
    
    return valRule;
}

#pragma - Rule Matching

- (BOOL) lessSpecificRuleMatches:(LTTriggerSetValRule *)matchRule
{
    /* Returns true if a less-specific rule or the default will 
     * have the same affect (condition, x and y and duration match)
     */
    
    /* Loop through all valRules to try and find match */
    for (LTTriggerSetValRule *valRule in self.valRules)
    {
        if (valRule == matchRule) continue;
        if ([matchRule hasTheSameEffectAs:valRule] 
            && [matchRule moreSpecificRule:valRule] == matchRule)
        {
            /* A value in the list matches for condition, 
             * type and values and is LESS specific that the 
             * supplied match rule. 
             */
            return YES;
        }
    }
    
    /* Check Default */
    if (matchRule.triggerType == self.defaultTriggerType && matchRule.duration == self.defaultDuration && matchRule.adminState == 0) 
    {
        /* Type and duration match, check values */
        if (self.valueType == VALTYPE_STRING)
        {
            if ([matchRule.xValue isEqualToString:self.defaultXValue]
                && (matchRule.triggerType != TRGTYPE_RANGE || [matchRule.yValue isEqualToString:self.defaultYValue]))
            { return YES; }
        }
        else
        {
            if ([matchRule.xValue floatValue] == [self.defaultXValue floatValue]
                && (matchRule.triggerType != TRGTYPE_RANGE || [matchRule.yValue floatValue] == [self.defaultYValue floatValue]))
            { return YES; }
        }
    }
    
    return NO;
}

@end
