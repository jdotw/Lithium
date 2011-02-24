//
//  LTriggerSet.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"

@interface LTTriggerSet : LTEntity 
{
    BOOL _apiUpdate;
    
    /* Values received via API 
     *
     * These are the 'original' parameters
     * received from Lithium core 
     */
    BOOL _apiApplied;
}

@property (nonatomic,assign) BOOL applied;
@property (nonatomic,assign) BOOL defaultApplied;
@property (nonatomic,readonly) NSMutableDictionary *appRuleDict;
@property (nonatomic,readonly) NSMutableArray *appRules;
@property (nonatomic,assign) BOOL ruleUpdateInProgress;
@property (nonatomic,readonly) BOOL setOrTriggersHaveChanged;

- (void) beginAPIUpdate;
- (void) endAPIUpdate;

- (void) sendRuleUpdatesForScopeObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName;

@end

@protocol LTTriggerSetDelegate <NSObject>

- (void) triggerSetUpdateDidFinish:(LTTriggerSet *)tset;
- (void) triggerSetUpdateDidFail:(LTTriggerSet *)tset;

@end

