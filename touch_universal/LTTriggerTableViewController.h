//
//  LTTriggerTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTTrigger;

@interface LTTriggerTableViewController : UITableViewController <UITextFieldDelegate>
{
    UISwitch *enabledSwitch;
    
    UISegmentedControl *conditionSegment;
    
    UITextField *xValueField;
    UITextField *yValueField;
}

@property (nonatomic,retain) LTTrigger *trg;
- (id)initWithTrigger:(LTTrigger *)trg;

@end
