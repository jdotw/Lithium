//
//  LTEntityIconView.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 4/11/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>

@class LTEntity;

@interface LTEntityIconView : UIView 
{
	
}

@property (nonatomic, assign) BOOL selected;
@property (nonatomic, retain) LTEntity *entity;

@end
