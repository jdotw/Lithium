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
    
    NSInteger indentLevel;
    CGFloat indentPerLevel;
}

@property (nonatomic,readonly) UILabel *textLabel;
@property (nonatomic,assign) NSInteger indentLevel;
@property (nonatomic,assign) CGFloat indentPerLevel;

@end
