//
//  LCReportMetricHistory.h
//  Lithium Console
//
//  Created by Liam Elliott on 16/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCMetricHistory.h"
#import "LCEntity.h"


@interface LCReportMetricHistory : LCMetricHistory {

	BOOL refreshComplete;
}

#pragma mark "Initialisation"
+(LCReportMetricHistory *)reportMetricHistoryWithMetric:(LCEntity *)entity 
									  withReferenceDate:(NSDate *)date 
										  andDataPeriod:(int)period;

-(LCReportMetricHistory *)init;
-(LCReportMetricHistory *)initWithMetric;
-(void)dealloc;

#pragma mark "Refresh Methods"
-(void)completeMetricHistoryRefresh;

-(NSDate *)findCriticalDate;

#pragma mark "Accessors"
-(BOOL)refreshComplete;
-(void)setRefreshComplete:(BOOL)value;

-(float) minimum;
-(NSString *)minimumString;
-(float) average;
-(NSString *)averageString;
-(float) maximum;
-(NSString *)maximumString;

@property (getter=refreshComplete,setter=setRefreshComplete:) BOOL refreshComplete;
@end 