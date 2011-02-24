//
//  LTTriggerSetValRule.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface LTTriggerSetValRule : NSObject 
{
}

@property (nonatomic,assign) int identifier;
@property (nonatomic,copy) NSString *siteName;
@property (nonatomic,copy) NSString *siteDesc;
@property (nonatomic,copy) NSString *devName;
@property (nonatomic,copy) NSString *devDesc;
@property (nonatomic,copy) NSString *objName;
@property (nonatomic,copy) NSString *objDesc;
@property (nonatomic,copy) NSString *trgName;
@property (nonatomic,copy) NSString *trgDesc;
@property (nonatomic,copy) NSString *xValue;
@property (nonatomic,copy) NSString *yValue;
@property (nonatomic,assign) int duration;
@property (nonatomic,assign) int triggerType;
@property (nonatomic,assign) int adminState;

- (LTTriggerSetValRule *) moreSpecificRule:(LTTriggerSetValRule *)otherRule;

@end
