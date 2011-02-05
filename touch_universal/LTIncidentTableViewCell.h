//
//  LTIncidentTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewCell.h"

@interface LTIncidentTableViewCell : LTTableViewCell 
{
    UIImageView *ticket;
    UILabel *incidentLabel;
    UIImageView *flag;
}

@property (nonatomic,readonly) UILabel *incidentLabel;

@end
