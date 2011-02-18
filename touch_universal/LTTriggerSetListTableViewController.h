//
//  LTTriggerSetListTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@class LTEntity, LTTriggerSetList;

@interface LTTriggerSetListTableViewController : LTTableViewController 
{
    LTTriggerSetList *tsetList;
}

@property (nonatomic,retain) LTEntity *metric;

@end
