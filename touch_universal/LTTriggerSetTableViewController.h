//
//  LTTriggerSetTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTriggerSet.h"

@class LTTriggerSetList, LTEntity;

@interface LTTriggerSetTableViewController : UITableViewController <LTTriggerSetDelegate, UIAlertViewDelegate>
{
    UISwitch *enabledSwitch;
    
    UISegmentedControl *objScopeSegment;
    UISegmentedControl *devScopeSegment;
    UISegmentedControl *siteScopeSegment;
    
    LTTriggerSetList *tsetList;
}

- (id)initWithMetric:(LTEntity *)metric;

@property (nonatomic,retain) LTTriggerSet *tset;
@property (nonatomic,retain) LTEntity *metric;

@end
