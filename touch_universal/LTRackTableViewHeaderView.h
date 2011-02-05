//
//  LTRackTableViewHeaderView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTRackTableViewHeaderView : UIView 
{
    UIImageView *tapeImageView;
    UILabel *textLabel;
    
    UIImageView *tlScrew;
    UIImageView *trScrew;
    UIImageView *blScrew;
    UIImageView *brScrew;
}

@property (nonatomic,readonly) UILabel *textLabel;

@end
