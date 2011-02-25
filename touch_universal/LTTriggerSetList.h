//
//  LTTriggerSetList.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTAPIRequest.h"

#define kLTTriggerSetListRefreshFinished @"kLTTriggerSetListRefreshFinished"

@class LTEntity;

@interface LTTriggerSetList : LTAPIRequest 
{
    NSMutableDictionary *childDict;
    NSMutableArray *children;
}

- (id) initWithMetric:(LTEntity *)metric;
- (void) refresh;

@property (nonatomic,retain) LTEntity *metric;
@property (nonatomic,readonly) NSMutableDictionary *childDict;
@property (nonatomic,readonly) NSMutableArray *children;

@end
