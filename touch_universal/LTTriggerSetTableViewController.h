//
//  LTTriggerSetTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 18/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTTriggerSet;

@interface LTTriggerSetTableViewController : UITableViewController {
    
}

- (id)initWithTriggerSet:(LTTriggerSet *)tset;

@property (nonatomic,retain) LTTriggerSet *tset;

@end
