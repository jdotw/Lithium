//
//  LCBrowserActionScriptsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 12/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserScriptsContentController.h"
#import "LCActionScriptList.h"

@interface LCBrowserActionScriptsContentController : LCBrowserScriptsContentController 
{
	/* Script List */
	LCActionScriptList *scriptList;
}

@property (nonatomic,retain) LCActionScriptList *scriptList;

@end
