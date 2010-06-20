//
//  LCModBScriptList.m
//  Lithium Console
//
//  Created by James Wilson on 13/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCModBScriptList.h"

#import "LCModBScript.h"

@implementation LCModBScriptList

#pragma mark "Sub-Class Methods (to be overidden)"

- (LCScript *) newScript
{
	return [LCModBScript new];
}

- (NSString *) scriptType
{ 
	return @"modb"; 
}

@end
