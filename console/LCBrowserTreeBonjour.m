//
//  LCBrowserTreeBonjour.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeBonjour.h"


@implementation LCBrowserTreeBonjour

#pragma mark "Constructors"

- (id) init
{
	[super init];
	
	self.isBrowserTreeLeaf = YES;
	
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Bonjour";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"web_16.tif"];
}

- (BOOL) selectable
{ return YES; }

@end
