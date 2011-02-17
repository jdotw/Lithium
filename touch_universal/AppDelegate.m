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
#import "LTEntityTableViewController.h"
#import "LTGroupTableViewController.h"
#import "LTFavoritesTableViewController.h"
#import "LTIncidentListTableViewController.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions 
{    	
	/* Create Core Deployment Dict */
	coreDeploymentDict = [[NSMutableDictionary dictionary] retain];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(coreDeploymentRefreshFinished:)
												 name:@"LTCoreDeploymentRefreshFinished"
											   object:nil];
	
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
	
	/* Initialize net browser */
	coreServiceBrowser = [[NSNetServiceBrowser alloc] init];
	coreServiceBrowser.delegate = self;
	[coreServiceBrowser searchForServicesOfType:@"_lithium._tcp" inDomain:@""];

	/* CHeck for at lease one core deployment */
	[NSTimer scheduledTimerWithTimeInterval:0.5
									 target:self
								   selector:@selector(initialDeploymentCheckTimerCallback:)
								   userInfo:nil
									repeats:NO];
	
	return YES;
}

- (void) initialDeploymentCheckTimerCallback:(NSTimer *)timer
{
	/* This is called 0.5s after the app loads to ensure that atleast one deployment was found */
	NSLog (@"Doing check for deployment");
	if (self.coreDeployments.count < 1)
	{
		LTCoreEditTableViewController *controller = [[LTCoreEditTableViewController alloc] initWithStyle:UITableViewStyleGrouped];
		UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
		[navController setModalPresentationStyle:UIModalPresentationFormSheet];
		navController.navigationBar.tintColor = [UIColor colorWithWhite:120.0/255.0 alpha:1.0];
		
		[tabBarController presentModalViewController:navController animated:YES];
		[controller release];
		[navController release];	
	}	
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

#pragma mark -
#pragma mark Bonjour Service Discovery

- (void) netServiceDidResolveAddress:(NSNetService *)netService
{
	NSDictionary *txtRecord = [NSNetService dictionaryFromTXTRecordData:netService.TXTRecordData];
	NSString *uuidString = [[[NSString alloc] initWithData:[txtRecord objectForKey:@"uuid"]
												  encoding:NSUTF8StringEncoding] autorelease];
	NSLog (@"Found %@ (%@) (UUID: %@)", netService, netService.hostName, uuidString);
	if (![coreDeploymentDict objectForKey:uuidString])
	{		
		/* Add new discovered core */
		LTCoreDeployment *core = [[LTCoreDeployment alloc] init];
		core.ipAddress = netService.hostName;
		core.desc = netService.hostName;
		core.enabled = YES;
		core.useSSL = NO;
		core.name = netService.hostName;
		core.discovered = YES;
		core.uuidString = uuidString;
		[self addCore:core];
	}
	else 
	{
		NSLog (@"Skipping known discovered core %@", netService.hostName, uuidString);
		
	}

}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
	NSLog (@"%@ didnt resolve %@", sender, errorDict);
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didFindService:(NSNetService *)netService moreComing:(BOOL)moreServicesComing
{
	netService.delegate = self;
	[netService resolveWithTimeout:2.0];
	[netService retain];
}

#pragma mark -
#pragma mark Tab Bar Delegate

- (void)tabBarController:(UITabBarController *)theTabBarController didSelectViewController:(UIViewController *)viewController
{
	NSLog (@"%@ selected %@", theTabBarController, viewController);
	if ([viewController isMemberOfClass:[LTEntityTableViewController class]])
	{
		/* Entity Table View Selected */
		LTEntityTableViewController *entityTVC = (LTEntityTableViewController *) viewController;
		[entityTVC refreshTouched:self];
	}
	else if ([viewController isMemberOfClass:[LTGroupTableViewController class]])
	{
		/* Groups Touched */
		for (LTCustomer *customer in [self valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{ 
			[customer.groupTree refresh];
		}		
	}
	else if ([viewController isMemberOfClass:[LTFavoritesTableViewController class]])
	{
		/* Favorites Touched 
		 *
		 * Nothing to do here, it's all local, perhaps just a refresh of each metric/graph? FIX 
		 */
	}
	else if ([viewController isMemberOfClass:[LTIncidentListTableViewController class]])
	{
		/* Incidents Touched */
		for (LTCustomer *customer in [self valueForKeyPath:@"coreDeployments.@unionOfArrays.children"])
		{ 
			[customer.incidentList refresh]; 
		}		
	}
}


#pragma mark -
#pragma mark Properties

@synthesize window;
@synthesize tabBarController;

@synthesize coreDeployments;
- (void) setCoreDeployments:(NSMutableArray *)value
{
	[coreDeployments release];
	coreDeployments = [value mutableCopy];
	[coreDeploymentDict removeAllObjects];
	for (LTCoreDeployment *core in coreDeployments)
	{
		if (core.uuidString) [coreDeploymentDict setObject:core forKey:core.uuidString];
	}
}
- (void) addCore:(LTCoreDeployment *)core
{
	[coreDeployments addObject:core];
	if (core.uuidString) [coreDeploymentDict setObject:core forKey:core.uuidString];
	[core refresh];
	[self saveCoreDeployments];
	[[NSNotificationCenter defaultCenter] postNotificationName:@"CoreDeploymentAdded" object:core];
}
- (void) removeCore:(LTCoreDeployment *)core
{
	[coreDeployments removeObject:core];
	if (core.uuidString) [coreDeploymentDict removeObjectForKey:core.uuidString];
	[self saveCoreDeployments];
}
- (void) saveCoreDeployments
{
	NSMutableArray *coresToSave = [NSMutableArray array];
	for (LTCoreDeployment *deployment in self.coreDeployments)
	{ if (!deployment.discovered) [coresToSave addObject:deployment]; }
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:coresToSave] forKey:@"coreDeployments"];
	[[NSUserDefaults standardUserDefaults] synchronize];
}
- (void) coreDeploymentRefreshFinished:(NSNotification *)note
{
	LTCoreDeployment *core = (LTCoreDeployment *) [note object];
	if (core.uuidString && ![coreDeploymentDict objectForKey:core.uuidString])
	{
		[coreDeploymentDict setObject:core forKey:core.uuidString];
	}
}

@synthesize authViewController;
@synthesize isActive;

@synthesize favoritesController, incidentsController;

@synthesize pushToken;

@synthesize operationQueue;


@end
