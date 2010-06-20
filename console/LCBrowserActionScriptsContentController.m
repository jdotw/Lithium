//
//  LCBrowserActionScriptsContentController.m
//  Lithium Console
//
//  Created by James Wilson on 12/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserActionScriptsContentController.h"


@implementation LCBrowserActionScriptsContentController

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithCustomer:initCustomer
						 inBrowser:initBrowser 
						  usingNib:@"ActionScriptsContent"
					 forScriptType:@"action"];
	if (!self) return nil;
	
	self.scriptList = [[[LCActionScriptList alloc] initWithCustomer:initCustomer task:nil] autorelease];
	[scriptList highPriorityRefresh];
	
	return self;
}

- (void) dealloc
{
	[scriptList release];
	[super dealloc];
}

@synthesize scriptList;


@end
