//
//  LTIconTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 12/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTIconTableViewController : UITableViewController 
{
	
}

@property (nonatomic, assign) CGFloat rowHeight;
@property (nonatomic, retain) IBOutlet UISlider *rowHeightSlider;

@end
