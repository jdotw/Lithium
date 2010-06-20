//
//  LCBrowserTreeVRacks.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeVRacks.h"

@implementation LCBrowserTreeVRacks

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Virtual Racks";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"documents_16.tif"];
}

- (NSString *) documentType
{
	return @"vrack";
}

@end
