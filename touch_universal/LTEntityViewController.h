//
//  LTEntityViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@class LTGraphLegendTableViewController, LTEntityTableViewController, LTEntity;

@interface LTEntityViewController : UIViewController 
{
	IBOutlet LTGraphLegendTableViewController *legendTableViewController;
	IBOutlet LTEntityTableViewController *entityTableViewController;
	IBOutlet UIScrollView *graphScrollView;
	
	LTEntity *entity;
	
	CATiledLayer *graphTiledLayer;
}

@property (nonatomic,retain) LTEntity *entity;

@end
