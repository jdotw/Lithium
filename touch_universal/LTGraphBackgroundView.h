//
//  LTGraphBackgroundView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 5/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTGraphBackgroundView : UIView 
{
}

@property (nonatomic, retain) IBOutlet UILabel *maxLabel;
@property (nonatomic, retain) IBOutlet UILabel *avgLabel;
@property (nonatomic, retain) IBOutlet UILabel *minLabel;	
@property (nonatomic,readonly) CGRect minLineRect;
@property (nonatomic,readonly) CGRect midLineRect;
@property (nonatomic,readonly) CGRect maxLineRect;

@end
