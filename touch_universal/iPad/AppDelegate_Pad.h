//
//  AppDelegate_Pad.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/06/10.
//  Copyright LithiumCorp 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

@class LTEntity, LTDeviceViewController;

@interface AppDelegate_Pad : AppDelegate 
{
	
}

@property (nonatomic, retain) IBOutlet UISplitViewController *splitViewController;
@property (nonatomic, retain) IBOutlet UINavigationController *detailNavigationController;
@property (nonatomic, retain) IBOutlet UINavigationItem *summaryNavItem;

- (void) displayEntityInDetailView:(LTEntity *)entity;

@end

