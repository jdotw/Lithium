//
//  LCBrowserModBScriptsContentController.m
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserModBScriptsContentController.h"


@implementation LCBrowserModBScriptsContentController

- (id) initWithCustomer:(LCCustomer *)initCustomer inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithCustomer:initCustomer
						 inBrowser:initBrowser 
						  usingNib:@"ModBScriptsContent"
					 forScriptType:@"modb"];
	if (!self) return nil;
	
	self.scriptList = [[[LCModBScriptList alloc] initWithCustomer:initCustomer task:nil] autorelease];
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
