//
//  LCActionScriptList.m
//  Lithium Console
//
//  Created by James Wilson on 14/05/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCActionScriptList.h"
#import "LCActionScript.h"

@implementation LCActionScriptList

#pragma mark "Sub-Class Methods (to be overidden)"

- (LCScript *) newScript
{
	return [LCActionScript new];
}

- (NSString *) scriptType
{ 
	return @"action"; 
}

@end
