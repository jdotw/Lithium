//
//  LCLayerNoHit.m
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCLayerNoHit.h"


@implementation LCLayerNoHit

- (CALayer *)hitTest:(CGPoint)thePoint
{
	return nil;
}

- (CAAnimation *)animationForKey:(NSString *)key
{
	CAAnimation *animation = [super animationForKey:key];
	return animation;
}

@end
