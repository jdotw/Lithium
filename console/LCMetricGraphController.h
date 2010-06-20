//
//  LCMetricGraphController.h
//  Lithium Console
//
//  Created by James Wilson on 28/02/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCXMLRequest.h"
#import "LCEntityArrayController.h"
#import "LCMetricGraphMetricItem.h"

@interface LCMetricGraphController : NSObject 
{
	/* Metrics */
	NSMutableArray *metricItems;
	
	/* Related Objects */
	NSResponder *undoResponder;

	/* Graph */
	NSPDFImageRep *graphPDFRep;
	NSString *imageURLString;
	float minValue;
	float maxValue;
	
	/* Baseline */
	NSPDFImageRep *baselinePDFRep;
	NSImage *baselineImage;
	float baselineMinValue;
	float baselineMaxValue;
	
	/* User variables */
	NSDate *referenceDate;
	NSString *referenceDateString;
	int graphPeriod;
	BOOL userCanSelectReferenceDate;
	int baselineType;
	int forwardAndBackMode;
	BOOL getMinMaxAvgValues;
	
	/* Graph refresh variables */
	LCXMLRequest *refreshXMLRequest;
	LCXMLRequest *baselineXMLRequest;
	NSMutableString *curXMLString;
	BOOL refreshInProgress;	
	NSMutableArray *xmlImagePaths;
	NSURLConnection *urlConn;
	NSURLConnection *baselineConn;
	BOOL parsingBaseline;
	NSMutableData *receivedData;
	NSMutableData *baselineReceivedData;
	LCActivity *activity;
	NSDate *lastRefresh;
	
	/* Window title */
	NSString *windowTitle;
	
	/* Undo Support */
	BOOL undoEnabled;
}

#pragma mark "Metric Selection"
+ (NSArray *) graphableMetricsForEntities:(NSArray *)entityArray;

#pragma mark "Graph Generation"
- (void) refreshGraph:(int)priority;
- (void) cancelRefresh;

#pragma mark "Context Menu Methods"
- (IBAction) copyURLToClipBoardClicked:(id)sender;
- (IBAction) openURLInWebBrowserClicked:(id)sender;
- (IBAction) saveImageAsClicked:(id)sender;

#pragma mark "Graph Display Methods"
- (void) blankGraph;

#pragma mark "Window Title"
- (void) updateTitle;
@property (copy) NSString *windowTitle;

#pragma mark "Metric Manipulation"
@property (readonly) NSMutableArray *metricItems;
- (void) insertObject:(LCMetricGraphMetricItem *)item inMetricItemsAtIndex:(unsigned int)index;
- (void) removeObjectFromMetricItemsAtIndex:(unsigned int)index;
- (void) removeAllMetricItems;
- (void) addMetric:(LCMetric *)metric;
- (void) addMetricsFromArray:(NSArray *)array;

#pragma mark "Timeframe Adjustment"
- (IBAction) forwardAndBackClicked:(id)sender;
@property (assign) int forwardAndBackMode;

#pragma mark "Accessor Methods"
@property (assign) BOOL refreshInProgress;
@property (copy) NSDate *referenceDate;
@property (copy) NSString *referenceDateString;
@property (assign) int graphPeriod;
@property (assign) BOOL userCanSelectReferenceDate;
@property (assign) int baselineType;
@property (assign) float minValue;
@property (assign) float maxValue;
@property (assign) float baselineMinValue;
@property (assign) float baselineMaxValue;
@property (copy) NSString *imageURLString;
@property (copy) NSPDFImageRep *graphPDFRep;
@property (copy) NSPDFImageRep *baselinePDFRep;
@property (assign) NSResponder *undoResponder;
@property (copy) NSDate *lastRefresh;
@property (assign) BOOL undoEnabled;
@property (assign) BOOL getMinMaxAvgValues;

@end
