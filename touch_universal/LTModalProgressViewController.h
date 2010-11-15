//
//  LTModalProgressViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTModalProgressViewController : UIViewController 
{
	IBOutlet UILabel *progressLabel;
	BOOL hasAppeared;
	BOOL shouldDismissWhenAppears;
	
	LTEntity *entity;
}

- (id) initWithEntity:(LTEntity *)initEntity;
@property (nonatomic,retain) LTEntity *entity;

@end
