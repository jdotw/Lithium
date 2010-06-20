//
//  LTMetricHistoryList.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"
#import "LTMetricValue.h"
#import "LTAPIRequest.h"

@interface LTMetricHistoryList : LTAPIRequest <NSXMLParserDelegate>
{
	LTEntity *metric;
	
	NSMutableArray *values;
	
	NSMutableString *curXmlString;
	LTMetricValue *curValue;	
	int valueIndex;
	
	float minValue;
	float avgValue;
	float maxValue;
	BOOL hasRealData;
}

- (void) refresh;

@property (readonly) NSMutableArray *values;
@property (retain) LTEntity *metric;
@property (assign) float minValue;
@property (assign) float avgValue;
@property (assign) float maxValue;
@property (assign) BOOL hasRealData;

@end
