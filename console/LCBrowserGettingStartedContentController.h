//
//  LCBrowserGettingStartedContentController.h
//  Lithium Console
//
//  Created by James Wilson on 23/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"

@interface LCBrowserGettingStartedContentController : LCBrowser2ContentViewController 
{
	LCBrowser2Controller *browser;
}

@property (nonatomic, assign) LCBrowser2Controller *browser;

@end
