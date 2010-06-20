//
//  LCAnalysisMetric.h
//  Lithium Console
//
//  Created by Liam Elliott on 29/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCMetric.h"
#import "LCMetricHistory.h"
#import "LCGeneralMatrix.h"


@interface LCAnalysisMetric : NSObject 
{
	/* Entity Details */
	LCMetric *metric;
	LCMetricHistory *entityHistory;
	
	/* triggers */
	NSMutableArray *triggers;
	
	/* Y Matrix = Y Values; ie: TimeStamps*/ 
    LCGeneralMatrix *Ymin;
	LCGeneralMatrix *Yavg;
	LCGeneralMatrix * Ymax;

	/* A Matrix = X Values and b constant; ie: values, 1 */
	LCGeneralMatrix *Amatrix;
	
	/* Recorded Values */
	NSMutableArray *minValues;
	NSMutableArray *avgValues;
	NSMutableArray * maxValues;
	
	/* Refresh Variables */
	BOOL refreshInProgress;
	NSDate *referenceDate;
	
	int datePeriod;
}

#pragma mark "Initialisation"
+(id) analysisMetricWithMetric: (LCMetric *)initMetric;
-(id) initWithMetric:(LCMetric *)initMetric;
-(id) init;
-(void) dealloc;

#pragma mark "Accessors"
@property (retain) LCMetric *metric;
@property (readonly) LCEntity *entity;
@property (assign) BOOL refreshInProgress;
@property (readonly) NSMutableArray *triggers;
@property (copy) NSDate *referenceDate;
@property (assign) int datePeriod;

#pragma mark "Populate and Process Matrices"
-(void) populateAMatrixWithValues:(NSArray *)values;
-(void) populateMatrix:(LCGeneralMatrix *)matrix withValues:(NSArray *)values;
-(LCGeneralMatrix *) calculateResultsFor:(LCGeneralMatrix *)Y andMatrix:(LCGeneralMatrix *)A;

#pragma mark "Process Metric"
-(void)processMetricForPeriod:(int)datePeriod atDate:(NSDate *)referenceDate;
-(void)processMetricForRows:(NSMutableArray *)rows;
-(void)processMetric;

@end
