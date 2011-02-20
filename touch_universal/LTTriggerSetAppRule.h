//
//  LTTriggerSetAppRule.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface LTTriggerSetAppRule : NSObject {
    
}

@property (nonatomic,assign) int identifier;
@property (nonatomic,copy) NSString *siteName;
@property (nonatomic,copy) NSString *siteDesc;
@property (nonatomic,copy) NSString *devName;
@property (nonatomic,copy) NSString *devDesc;
@property (nonatomic,copy) NSString *objName;
@property (nonatomic,copy) NSString *objDesc;
@property (nonatomic,assign) int applyFlag;

@end
