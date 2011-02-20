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

@interface LTTrigger : LTEntity {
    
}

@property (nonatomic,assign) int valueType;
@property (nonatomic,assign) int triggerType;
@property (nonatomic,assign) int effect;
@property (nonatomic,copy) NSString *xValue;
@property (nonatomic,copy) NSString *yValue;
@property (nonatomic,assign) int duration;
@property (nonatomic,readonly) NSString *conditionString;
@end
