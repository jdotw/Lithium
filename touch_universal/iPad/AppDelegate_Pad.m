//
//  AppDelegate_Pad.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/06/10.
//  Copyright LithiumCorp 2010. All rights reserved.
//

#import "AppDelegate_Pad.h"

@implementation AppDelegate_Pad

@synthesize splitViewController;

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


@end
