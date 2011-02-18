//
//  LTTrigger.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"

@interface LTTrigger : LTEntity {
    
}

@property (nonatomic,assign) int valueType;
@property (nonatomic,assign) int triggerType;
@property (nonatomic,assign) int effect;
@property (nonatomic,copy) NSString *xValue;
@property (nonatomic,copy) NSString *yValue;
@property (nonatomic,assign) int duration;

@end
