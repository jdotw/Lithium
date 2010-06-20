//
//  LCDateAgoTranformer.m
//  Lithium Console
//
//  Created by James Wilson on 5/02/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import "LCDateAgoTranformer.h"


@implementation LCDateAgoTranformer

+ (Class) transformedValueClass
{
	return [NSString class];
}

+ (BOOL) allowsReverseTransformation
{
	return NO;
}

- (NSString *) transformedValue:(NSDate *)date
{
	NSTimeInterval interval = [[NSDate date] timeIntervalSinceDate:date];
	int minutes = (int) interval / 60;
	
	if (minutes < 60) return [NSString stringWithFormat:@"%i mins ago", minutes];
	else if (minutes < 60 * 24) return [NSString stringWithFormat:@"%i hours ago", minutes / 60];
	else return [NSString stringWithFormat:@"%i days ago", minutes / (60 * 24)];
}


@end
