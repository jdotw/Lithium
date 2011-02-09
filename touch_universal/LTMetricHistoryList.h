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

@property (nonatomic,readonly) NSMutableArray *values;
@property (nonatomic,retain) LTEntity *metric;
@property (nonatomic,assign) float minValue;
@property (nonatomic,assign) float avgValue;
@property (nonatomic,assign) float maxValue;
@property (nonatomic,assign) BOOL hasRealData;

@end
