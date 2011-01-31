//
//  AppDelegate.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 6/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

@class LTAuthenticationTableViewController, LTFavoritesTableViewController, LTCoreDeployment, LTIncidentListTableViewController, LTEntity;

#define kDeviceListGroupByLocation @"LTSetupDeviceListGroupByLocation"

@interface AppDelegate : NSObject <UIApplicationDelegate, NSNetServiceBrowserDelegate, NSNetServiceDelegate, UITabBarDelegate>
{
	/* UI Elements */
    UIWindow *window;
    IBOutlet UITabBarController *tabBarController;
	NSMutableArray *coreDeployments;
	LTAuthenticationTableViewController *authViewController;
	IBOutlet LTFavoritesTableViewController *favoritesController;
	IBOutlet LTIncidentListTableViewController *incidentsController;
	IBOutlet UITabBarItem *incidentsTabBarItem;
	
	/* State */
	BOOL isActive;

	/* Push Notifications */
	NSData *pushToken;

	/* Operation Queue */
	NSOperationQueue *operationQueue;
	NSMutableArray *entityRefreshQueue;
	
	/* NetService Browser */
	NSNetServiceBrowser *coreServiceBrowser;
	
	/* Core Deployments */
	NSMutableDictionary *coreDeploymentDict;	/* Keyed by UUID String */
	
	/* Refresh Timers */
	NSTimer *incidentCountRefreshTimer;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UITabBarController *tabBarController;
@property (retain) LTAuthenticationTableViewController *authViewController;
@property (assign) LTFavoritesTableViewController *favoritesController;
@property (assign) LTIncidentListTableViewController *incidentsController;


@property (copy) NSMutableArray *coreDeployments;
- (void) addCore:(LTCoreDeployment *)core;
- (void) removeCore:(LTCoreDeployment *)core;
- (void) saveCoreDeployments;
- (void) entityRefreshDidBegin:(LTEntity *)entity;
- (void) entityRefreshDidFinish:(LTEntity *)entity;

@property (assign) BOOL isActive;

@property (copy) NSData *pushToken;

@property (readonly) NSOperationQueue *operationQueue;
@property (readonly) NSMutableArray *entityRefreshQueue;

@end
