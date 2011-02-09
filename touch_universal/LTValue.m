//
//  LTValue.m
//  LithiumTouchUniversal
//
//  Created by James Wilson on 9/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import "LTValue.h"


@implementation LTValue

- (unsigned long long) unsignedLongLongValue
{
    return (unsigned long long) [self longLongValue];
}

@end
