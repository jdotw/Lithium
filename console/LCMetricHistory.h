//
//  LCMetricHistory.h
//  Lithium Console
//
//  Created by James Wilson on 18/07/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCMetric.h"
#import "LCXMLRequest.h"
#import "LCMetricHistoryRow.h"

@interface LCMetricHistory : NSObject 
{
	/* Criteria */
	LCMetric *metric;
	int graphPeriod;
	NSDate *referenceDate;
	
	/* Properties */
	NSMutableDictionary *properties;
	
	/* Rows */
	NSMutableArray *rows;
	NSMutableDictionary *rowDict;
	
	/* Summary values */
	float minimum;
	float average;
	float maximum;
	NSNumber *minimumNumber;
	NSNumber *averageNumber;
	NSNumber *maximumNumber;
	NSString *minimumString;
	NSString *averageString;
	NSString *maximumString;
	
	/* 95th Percentile values */
	float min95thPercentile;
	float avg95thPercentile;
	float max95thPercentile;
	BOOL discardNanFor95th;
	
	/* Operational Variables */
	LCXMLRequest *refreshXMLRequest;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;	
	LCMetricHistoryRow *currentRow;
	int currentCol;
}

#pragma mark "Initialisation"
+ (id) historyWithMetric:(LCMetric *)initMetric;
- (id) initWithMetric:(LCMetric *)initMetric;

#pragma mark "Refresh Method"
- (void) refresh:(int)priority;
- (void) cancelRefresh;
- (void) XMLRequestPreParse:(id)sender;
- (void) parser:(NSXMLParser *)parser didStartElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname attributes:(NSDictionary *)attribdict;
- (void) parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
- (void) parser:(NSXMLParser *)parser didEndElement:(NSString *)element namespaceURI:(NSString *)namespace qualifiedName:(NSString *)qname;
- (void) XMLRequestFinished:(LCXMLRequest *)sender;

#pragma mark "95th Percentile"
- (void) calculate95thPercentile;
- (float) min95thPercentile;
- (void) setMin95thPercentile:(float)value;
- (NSNumber *) min95thPercentileNumber;
- (float) avg95thPercentile;
- (void) setAvg95thPercentile:(float)value;
- (NSNumber *) avg95thPercentileNumber;
- (float) max95thPercentile;
- (void) setMax95thPercentile:(float)value;
- (NSNumber *) max95thPercentileNumber;
- (BOOL) discardNanFor95th;
- (void) setDiscardNanFor95th:(BOOL)value;

#pragma mark "Accessor Methods"
@property (nonatomic,retain) LCMetric *metric;
@property (nonatomic, assign) BOOL refreshInProgress;
@property (nonatomic,copy) NSDate *referenceDate;
@property (nonatomic, assign) int graphPeriod;
@property (nonatomic, assign) float minimum;
@property (nonatomic,copy) NSNumber *minimumNumber;
@property (nonatomic,copy) NSString *minimumString;
@property (nonatomic, assign) float average;
@property (nonatomic,copy) NSNumber *averageNumber;
@property (nonatomic,copy) NSString *averageString;
@property (nonatomic, assign) float maximum;
@property (nonatomic,copy) NSNumber *maximumNumber;
@property (nonatomic,copy) NSString *maximumString;
@property (readonly) NSMutableArray *rows;
- (void) insertObject:(LCMetricHistoryRow *)row inRowsAtIndex:(unsigned int)index;
- (void) removeObjectFromRowsAtIndex:(unsigned int)index;

@end
