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
    UIView *topTapeView;
    UIView *bottomTapeView;
    
    NSInteger row;
    BOOL lastRow;
    BOOL firstRow;
}

@property (nonatomic,readonly) UILabel *incidentLabel;
@property (nonatomic,assign) NSInteger row;
@property (nonatomic,assign) BOOL lastRow;
@property (nonatomic,assign) BOOL firstRow;

@end
