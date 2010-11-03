//
//  LTEntityIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 3/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTEntityIconViewController : UIViewController 
{
	IBOutlet UIImageView *imageView;
	IBOutlet UILabel *label;
	LTEntity *_entity;
	
	BOOL selected;
	
	id delegate;	
}

- (id) initWithNibName:(NSString *)nibName entity:(LTEntity *)entity;

@property (retain, nonatomic) LTEntity *entity;
@property (assign, nonatomic) BOOL selected;
@property (assign, nonatomic) id delegate;


@end
