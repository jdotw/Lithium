//
//  LTTriggerSetTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTriggerSet.h"

@interface LTTriggerSetTableViewController : UITableViewController <LTTriggerSetDelegate>
{
    UISwitch *enabledSwitch;
    
    UISegmentedControl *objScopeSegment;
    UISegmentedControl *devScopeSegment;
    UISegmentedControl *siteScopeSegment;
}

- (id)initWithTriggerSet:(LTTriggerSet *)tset;

@property (nonatomic,retain) LTTriggerSet *tset;

@end
