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

@synthesize splitViewController, detailNavigationController, deviceNavigationItem, deviceViewController;

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
	/* Called when something is selected from the 'master' side
	 * of the split view controller to be displayed in the 'detail'
	 * side of the splitview controller
	 *
	 * The device view controller is persistent, so we just reset
	 * the entity to display. 
	 */
	
	
	[self.deviceViewController displayDevice:entity.device withInitialSelection:entity];
}

@end
