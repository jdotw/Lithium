//
//  LTContainerIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTEntityIconViewController.h"

@class LTEntity;

@interface LTContainerIconViewController : LTEntityIconViewController 
{
}

- (id) initWithContainer:(LTEntity *)container;

@end
