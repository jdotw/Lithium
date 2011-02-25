//
//  LTTrigger.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"

#define TRGTYPE_EQUAL 1                         /* Metric = Trigger */
#define TRGTYPE_LT 2                            /* Metric < Trigger */
#define TRGTYPE_GT 3                            /* Metric > Trigger */
#define TRGTYPE_NOTEQUAL 4                      /* Metric != Trigger */
#define TRGTYPE_RANGE 5                         /* xValue <= Trigger < yValue */

#define VALTYPE_INTEGER 1                   /* Signed 32bit integer value */
#define VALTYPE_COUNT 2                     /* Unsigned 32bit integer wrapping value */
#define VALTYPE_GAUGE 3                     /* Unsigned 32bit integer non-wrapping value */
#define VALTYPE_FLOAT 4                     /* Floating point value */
#define VALTYPE_STRING 5                    /* NULL terminated string value */
#define VALTYPE_OID 6                       /* OID value */
#define VALTYPE_IP 7                        /* IP Address */
#define VALTYPE_INTERVAL 8                  /* A time interval in seconds */
#define VALTYPE_HEXSTRING 9                 /* A time interval in seconds */
#define VALTYPE_COUNT_HEX64 10              /* Unsigned 64bit integer wrapping value derived from hex string */
#define VALTYPE_GAUGE_HEX64 11              /* Unsigned 64bit integer non-wrapping value derived from hex string */
#define VALTYPE_COUNT64 12                  /* Unsigned 64bit integer non-wrapping value */
#define VALTYPE_DATA 100                    /* Opaque data value */

@class LTTriggerSetValRule;

@interface LTTrigger : LTEntity 
{
    BOOL _apiUpdate;
    
    /* Values received via API 
     *
     * These are the 'original' parameters
     * received from Lithium core 
     */
    int _apiAdminState;
    int _apiTriggerType;
    NSString *_apiXValue;
    NSString *_apiYValue;
    int _apiDuration;
}

@property (nonatomic,assign) int valueType;
@property (nonatomic,assign) int triggerType;
@property (nonatomic,assign) int defaultTriggerType;
@property (nonatomic,assign) int effect;
@property (nonatomic,copy) NSString *xValue;
@property (nonatomic,copy) NSString *defaultXValue;
@property (nonatomic,copy) NSString *yValue;
@property (nonatomic,copy) NSString *defaultYValue;
@property (nonatomic,assign) int duration;
@property (nonatomic,assign) int defaultDuration;
@property (nonatomic,readonly) NSString *conditionString;
@property (nonatomic,readonly) NSString *defaultConditionString;
@property (nonatomic,readonly) NSMutableDictionary *valRuleDict;
@property (nonatomic,readonly) NSMutableArray *valRules;
@property (nonatomic,readonly) BOOL triggerHasChanged;


- (void) beginAPIUpdate;
- (void) endAPIUpdate;
- (void) restoreDefaults;

- (LTTriggerSetValRule *) ruleToUpdateForChangesInObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName;
- (LTTriggerSetValRule *) ruleToDeleteForChangesInObject:(NSString *)objName device:(NSString *)devName site:(NSString *)siteName;
- (BOOL) lessSpecificRuleMatches:(LTTriggerSetValRule *)matchRule;

@end
