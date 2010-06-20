//
//  NSDate-Ago.m
//  StickyBeak
//
//  Created by James Wilson on 26/11/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "NSDate-Ago.h"


@implementation NSDate (Ago)

- (NSString *) agoString
{
	NSTimeInterval interval = [[NSDate date] timeIntervalSinceDate:self];
	int minutes = (int) interval / 60;
	
	if (minutes < 60) return [NSString stringWithFormat:@"%i mins ago", minutes];
	else if (minutes < 60 * 24) return [NSString stringWithFormat:@"%i hours ago", minutes / 60];
	else return [NSString stringWithFormat:@"%i days ago", minutes / (60 * 24)];
}

- (NSString *) agoShortString
{
	NSTimeInterval interval = [[NSDate date] timeIntervalSinceDate:self];
	int minutes = (int) interval / 60;
	
	if (minutes < 60) return [NSString stringWithFormat:@"%imin", minutes];
	else if (minutes < 60 * 24) return [NSString stringWithFormat:@"%ihr", minutes / 60];
	else if ((minutes / (60 * 24)) == 1)return [NSString stringWithFormat:@"%iday", minutes / (60 * 24)];	
	else return [NSString stringWithFormat:@"%idays", minutes / (60 * 24)];	
}

@end
