//
//  LTTriggerSetAppRuleList.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTAPIRequest.h"

#define kLTTriggerSetAppRuleListRefreshFinished @"kLTTriggerSetAppRuleListRefreshFinished"

@class LTTriggerSet;

@interface LTTriggerSetAppRuleList : LTAPIRequest 
{
    NSMutableDictionary *ruleDict;
    NSMutableArray *rules;
}

- (id) initWithTriggerSet:(LTTriggerSet *)tset;
- (void) refresh;

@property (nonatomic,retain) LTTriggerSet *tset;
@property (nonatomic,readonly) NSMutableDictionary *ruleDict;
@property (nonatomic,readonly) NSMutableArray *rules;

@end
