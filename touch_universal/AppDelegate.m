//
//  AppDelegate.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "AppDelegate.h"
#import "LTCoreDeployment.h"
#import "LTCoreEditTableViewController.h"
#import "LTAuthenticationTableViewController.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions 
{    	
	/* Create op queue */
	self.operationQueue = [[[NSOperationQueue alloc] init] autorelease];
	
	/* Create Auth Controller */
	authViewController = [[LTAuthenticationTableViewController alloc] initWithNibName:@"AuthenticationView" bundle:nil];	
	
	/* Begin orientation notifications */
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	
	/* Register for push */
#ifndef DEMO
	NSLog (@"Registering for PUSH notifications");
	[[UIApplication sharedApplication] registerForRemoteNotificationTypes:UIRemoteNotificationTypeBadge|UIRemoteNotificationTypeSound|UIRemoteNotificationTypeAlert];
#endif
	
	/* Load coreDeployments */
#ifdef DEMO
	self.coreDeployments = [NSMutableArray array];
	LTCoreDeployment *demoDeployment = [LTCoreDeployment new];
	demoDeployment.desc = @"L5.0 Demo Deployment";
	demoDeployment.ipAddress = @"203.206.220.8";
	demoDeployment.name = demoDeployment.ipAddress;
	demoDeployment.enabled = YES;
	[self.coreDeployments addObject:demoDeployment];	
	[demoDeployment refresh];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"CoreDeploymentAdded" object:self];
#else
	if ([[NSUserDefaults standardUserDefaults] objectForKey:@"coreDeployments"])
	{
		NSArray *savedCores = [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"coreDeployments"]];
		self.coreDeployments = [NSMutableArray arrayWithArray:savedCores];
		for (LTCoreDeployment *core in self.coreDeployments)
		{
			[core refresh];
		}
		[[NSNotificationCenter defaultCenter] postNotificationName:@"CoreDeploymentAdded" object:self];
	}
	else if (!self.coreDeployments)
	{ self.coreDeployments = [NSMutableArray array]; }	
#endif
	
    /* Display */
	[window addSubview:tabBarController.view];
    [window makeKeyAndVisible];

	/* CHeck for at lease one core deployment */
	if (self.coreDeployments.count < 1)
	{
		LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
		UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
#ifdef UIModalPresentationFormSheet
		[navController setModalPresentationStyle:UIModalPresentationFormSheet];
#endif
		navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
		
		[tabBarController presentModalViewController:navController animated:YES];
		[controller release];
		[navController release];	
	}
	
	
	return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	self.isActive = YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	self.isActive = NO;
}


- (void)dealloc {
    [window release];
    [super dealloc];
}

#pragma mark "Push Notifications"

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
	self.pushToken = deviceToken;
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
	NSLog (@"FAILED to register: %@", error);
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
	//	NSLog (@"PUSH: %@", userInfo);
}

#pragma mark "Properties"

@synthesize window;
@synthesize tabBarController;

@synthesize coreDeployments;
- (void) setCoreDeployments:(NSMutableArray *)value
{
	[coreDeployments release];
	coreDeployments = [value mutableCopy];
}
- (void) addCore:(LTCoreDeployment *)core
{
	[coreDeployments addObject:core];
	[core refresh];
	[self saveCoreDeployments];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"CoreDeploymentAdded" object:self];
}
- (void) removeCore:(LTCoreDeployment *)core
{
	[coreDeployments removeObject:core];
	[self saveCoreDeployments];
}
- (void) saveCoreDeployments
{
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:self.coreDeployments] forKey:@"coreDeployments"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

@synthesize authViewController;
@synthesize isActive;

@synthesize favoritesController;

@synthesize pushToken;

@synthesize operationQueue;


@end
