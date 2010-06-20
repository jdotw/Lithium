//
//  LCBrowserTreeCases.m
//  Lithium Console
//
//  Created by James Wilson on 26/03/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeCases.h"


@implementation LCBrowserTreeCases

#pragma mark "Constructors"

- (LCBrowserTreeCases *) init
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
	return @"Cases";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"folder_16.tif"];
}

- (BOOL) selectable
{ return YES; }

@end
