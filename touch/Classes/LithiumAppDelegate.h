//
//  LithiumAppDelegate.h
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTAuthenticationTableViewController.h"
#import "LTCoreDeployment.h"
#import "LTFavoritesTableViewController.h"

@interface LithiumAppDelegate : NSObject <UIApplicationDelegate, UITabBarControllerDelegate> 
{
    UIWindow *window;
    UITabBarController *tabBarController;
	NSMutableArray *coreDeployments;
	LTAuthenticationTableViewController *authViewController;
	
	IBOutlet LTFavoritesTableViewController *favoritesController;
	
	BOOL isActive;
	
	NSData *pushToken;
	
	NSOperationQueue *operationQueue;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UITabBarController *tabBarController;

@property (copy) NSMutableArray *coreDeployments;
- (void) addCore:(LTCoreDeployment *)core;
- (void) removeCore:(LTCoreDeployment *)core;
- (void) saveCoreDeployments;

@property (retain) LTAuthenticationTableViewController *authViewController;

@property (assign) BOOL isActive;
@property (copy) NSData *pushToken;

@property (assign) LTFavoritesTableViewController *favoritesController;

@property (retain) NSOperationQueue *operationQueue;

@end
