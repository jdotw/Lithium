//
//  LCBrowserTreeScenes.m
//  Lithium Console
//
//  Created by James Wilson on 16/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeScenes.h"
#import "LCCustomer.h"
#import "LCCustomerList.h"

@implementation LCBrowserTreeScenes

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"State Scenes";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"pictures_16.tif"];
}

- (NSString *) documentType
{
	return @"scene";
}

@end
