//
//  LCBWRepInterface.h
//  Lithium Console
//
//  Created by James Wilson on 25/12/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCBWRepItem.h"
#import "LCMetricHistory.h"

@interface LCBWRepInterface : LCBWRepItem 
{
	LCMetricHistory *inHistory;
	bool inHistoryRefreshInProgress;
	LCMetricHistory *outHistory;
	bool outHistoryRefreshInProgress;
}

#pragma mark "Constructors"
+ (LCBWRepInterface *) interfaceItemWithEntity:(LCEntity *)initEntity;
- (LCBWRepInterface *) init;
- (LCBWRepInterface *) initWithEntity:(LCEntity *)initEntity;
- (LCBWRepItem *) initWithCoder:(NSCoder *)decoder;

#pragma mark "History Methods"
- (void) createHistoryObjects;

#pragma mark "KVO Methods"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context;

#pragma mark "Refresh"
- (void) refresh:(int)priority;

#pragma mark "Accessors"
- (NSString *) displayDescription;
- (NSNumber *) inMinimum;
- (NSNumber *) inAverage;
- (NSNumber *) inMaximum;
- (NSNumber *) outMinimum;
- (NSNumber *) outAverage;
- (NSNumber *) outMaximum;
- (void) setDiscardMissing:(BOOL)value;
- (void) setReferenceDate:(NSDate *)value;
- (void) setReferencePeriod:(int)value;
- (LCMetricHistory *) inMetricHistory;
- (LCMetricHistory *) outMetricHistory;

@property (retain,getter=inMetricHistory) LCMetricHistory *inHistory;
@property 	bool inHistoryRefreshInProgress;
@property (retain,getter=outMetricHistory) LCMetricHistory *outHistory;
@property 	bool outHistoryRefreshInProgress;
@end
