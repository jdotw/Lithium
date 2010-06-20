//
//  LCServiceScriptList.m
//  Lithium Console
//
//  Created by James Wilson on 7/06/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCServiceScriptList.h"


@implementation LCServiceScriptList

#pragma mark "Sub-Class Methods (to be overidden)"

- (LCScript *) newScript
{
	return [LCServiceScript new];
}

- (NSString *) scriptType
{ 
	return @"service"; 
}

@end
