//
//  LTObjectIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 2/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LTEntityIconViewController.h"

@class LTEntity;

@interface LTObjectIconViewController : LTEntityIconViewController 
{
}

- (id) initWithObject:(LTEntity *)object;

@end
