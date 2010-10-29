//
//  LCBrowserActionFilterContentController.h
//  Lithium Console
//
//  Created by James Wilson on 27/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h"
#import "LCCustomer.h"
#import "LCEntity.h"
#import "LCDevice.h"

@interface LCBrowserActionFilterContentController : LCBrowser2ContentViewController 
{
	/* Objects */
	LCCustomer *customer;
	LCBrowser2Controller *browser;
	
	/* UI Elements */
	IBOutlet NSTreeController *treeController;
	
	/* Selection */
	LCDevice *selectedDevice;
	NSArray *selectedDevices;	
	LCEntity *selectedEntity;
	NSArray *selectedEntities;	
}

#pragma mark "Constructors"
- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection (KVO Observable)"
@property (nonatomic, assign) LCEntity *selectedEntity;
@property (nonatomic,copy) NSArray *selectedEntities;
@property (nonatomic, assign) LCDevice *selectedDevice;
@property (nonatomic, copy) NSArray *selectedDevices;

#pragma mark "Properties"
@property (nonatomic, assign) LCBrowser2Controller *browser;
@property (nonatomic,retain) LCCustomer *customer;

@end

