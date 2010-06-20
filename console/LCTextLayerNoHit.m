//
//  LCTextLayerNoHit.m
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCTextLayerNoHit.h"


@implementation LCTextLayerNoHit

- (CALayer *)hitTest:(CGPoint)thePoint
{
	return nil;
}

@end
