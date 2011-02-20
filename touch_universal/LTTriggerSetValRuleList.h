//
//  LTTriggerSetValRuleList.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTAPIRequest.h"

#define kLTTriggerSetValRuleListRefreshFinished @"kLTTriggerSetValRuleListRefreshFinished"

@class LTTriggerSet, LTTrigger;

@interface LTTriggerSetValRuleList : LTAPIRequest 
{
    NSMutableDictionary *ruleDict;
    NSMutableArray *rules;    
}

- (id) initWithTriggerSet:(LTTriggerSet *)tset trigger:(LTTrigger *)trg;
- (void) refresh;

@property (nonatomic,retain) LTTriggerSet *tset;
@property (nonatomic,retain) LTTrigger *trg;
@property (nonatomic,readonly) NSMutableDictionary *ruleDict;
@property (nonatomic,readonly) NSMutableArray *rules;

@end
