//
//  AppDelegate_Pad.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/06/10.
//  Copyright LithiumCorp 2010. All rights reserved.
//

#import "AppDelegate_Pad.h"
#import "LTDeviceViewController.h"

@implementation AppDelegate_Pad

@synthesize splitViewController, detailNavigationController, summaryNavItem;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions 
{   
	[super application:application didFinishLaunchingWithOptions:launchOptions];
	
    // Override point for customization after application launch
	[self.window addSubview:splitViewController.view];
    [self.window makeKeyAndVisible];
	
	return YES;
}


- (void)dealloc 
{
    [super dealloc];
}

- (void) displayEntityInDetailView:(LTEntity *)entity
{
	/* Pushes a device view onto the detail navigation controller */
	BOOL animatePush = YES;
	if ([[detailNavigationController topViewController] class] == [LTDeviceViewController class])
	{
		/* Top controller is already a device view, dont animate */
		[self.detailNavigationController popViewControllerAnimated:NO];
		animatePush = NO;
	}
	
	
	LTDeviceViewController *devVC = [[LTDeviceViewController alloc] initWithEntityToHighlight:entity];
	[self.detailNavigationController pushViewController:devVC animated:animatePush];
	[devVC release];
}

@end
