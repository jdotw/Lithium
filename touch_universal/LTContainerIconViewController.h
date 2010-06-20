//
//  LTContainerIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 7/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTContainerIconViewController : UIViewController 
{
	IBOutlet UIImageView *imageView;
	IBOutlet UILabel *label;
	LTEntity *_container;
	
	BOOL selected;
	
	id delegate;
}

- (id) initWithContainer:(LTEntity *)container;
- (IBAction) iconTapped:(id)sender;

@property (retain, nonatomic) LTEntity *container;
@property (assign, nonatomic) BOOL selected;
@property (assign, nonatomic) id delegate;

@end
