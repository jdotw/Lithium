//
//  LithiumAppDelegate.m
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "LithiumAppDelegate.h"
#import "LTCoreDeployment.h"
#import "NSData-Base64.h"
#import "LTCoreEditTableViewController.h"
#import <CrashReporter/CrashReporter.h>
#import "CrashReportSender.h"
#include "TargetConditionals.h"


#define CRASH_REPORTER_URL [NSURL URLWithString:@"https://secure.lithiumcorp.com.au/crash/crash_v200.php"]

@implementation LithiumAppDelegate

#pragma mark "Constructors"

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	/* Report crashes -- Do this as early as possible */
#if !(TARGET_IPHONE_SIMULATOR)
	[[CrashReportSender sharedCrashReportSender] sendCrashReportToURL:CRASH_REPORTER_URL 
                                                             delegate:self 
                                                     activateFeedback:YES];		
#endif
	
	/* Create op queue */
	self.operationQueue = [[[NSOperationQueue alloc] init] autorelease];

	/* Create Auth Controller */
	authViewController = [[LTAuthenticationTableViewController alloc] initWithNibName:@"AuthenticationView" bundle:nil];	
	
	/* Begin orientation notifications */
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
}

- (void) dealloc 
{
	[operationQueue release];
    [tabBarController release];
    [window release];
    [super dealloc];
}

#pragma mark "App Delegate Methods"

- (void)applicationDidFinishLaunching:(UIApplication *)application 
{
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
	
    // Add the tab bar controller's current view as a subview of the window
    [window addSubview:tabBarController.view];
	
	/* CHeck for at lease one core deployment */
	if (self.coreDeployments.count < 1)
	{
		LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithNibName:@"CoreEditView" bundle:nil];
		UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
		navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
		[tabBarController presentModalViewController:navController animated:YES];
		[controller release];
		[navController release];	
	}
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
	return NO;
}

/*
// Optional UITabBarControllerDelegate method
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController {
}
*/

/*
// Optional UITabBarControllerDelegate method
- (void)tabBarController:(UITabBarController *)tabBarController didEndCustomizingViewControllers:(NSArray *)viewControllers changed:(BOOL)changed {
}
*/

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	self.isActive = YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	self.isActive = NO;
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

