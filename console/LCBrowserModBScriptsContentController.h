//
//  LCBrowserModBScriptsContentController.h
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowserScriptsContentController.h"
#import "LCModBScriptList.h"

@interface LCBrowserModBScriptsContentController : LCBrowserScriptsContentController 
{
	LCModBScriptList *scriptList;
}

@property (retain) LCModBScriptList *scriptList;

@end
