//
//  LCMetricHistoryRow.h
//  Lithium Console
//
//  Created by James Wilson on 18/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCMetricHistoryRow : NSObject 
{
	/* Properties */
	NSDate *timeStamp;
	float minimum;
	float average;
	float maximum;
}


#pragma mark "Initialisation"
+ (LCMetricHistoryRow *) row;

#pragma mark Accessors
@property (nonatomic,copy) NSDate *timeStamp;
@property (nonatomic, assign) float minimum;
@property (nonatomic, assign) float average;
@property (nonatomic, assign) float maximum;
- (NSNumber *) minimumNumber;
- (NSNumber *) averageNumber;
- (NSNumber *) maximumNumber;

@end
