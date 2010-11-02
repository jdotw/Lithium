//
//  LTObjectIconViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 2/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTObjectIconViewController : UIViewController 
{
	IBOutlet UIImageView *imageView;
	IBOutlet UILabel *label;
	LTEntity *_object;
	
	BOOL selected;
	
	id delegate;
}

- (id) initWithObject:(LTEntity *)object;
- (IBAction) iconTapped:(id)sender;

@property (nonatomic, retain) LTEntity *object;
@property (nonatomic, assign) BOOL selected;
@property (nonatomic, assign) id delegate;

@end
