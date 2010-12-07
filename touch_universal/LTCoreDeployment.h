//
//  LTCoreDeployment.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <SystemConfiguration/SystemConfiguration.h>

#import "LTCustomer.h"
#import "LTEntity.h"

@interface LTCoreDeployment : LTEntity
{
	/* Reachability */
	SCNetworkReachabilityRef reachabilityRef;
	
}

#pragma mark "Properties"
@property (nonatomic, assign) BOOL enabled;
@property (nonatomic, assign) BOOL useSSL;
@property (nonatomic, readonly) BOOL reachable;
@property (nonatomic, assign) BOOL discovered;

@end
