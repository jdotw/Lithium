//
//  LCBrowserServiceScriptsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 12/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserScriptsContentController.h"
#import "LCServiceScriptList.h"

@interface LCBrowserServiceScriptsContentController : LCBrowserScriptsContentController 
{
	/* Script list */
	LCServiceScriptList *scriptList;
}

@property (retain) LCServiceScriptList *scriptList;

@end
