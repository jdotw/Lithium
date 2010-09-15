//
//  LTMetricGraphRequest.h
//  Lithium
//
//  Created by James Wilson on 31/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTEntity.h"
#import "LTAPIRequest.h"

@interface LTMetricGraphRequest : LTAPIRequest <NSXMLParserDelegate>
{
	/* Request Data */
	NSMutableArray *metrics;
	CGSize size;
	NSDate *referenceDate;
	int startSec;
	int endSec;
	BOOL synchronous;
	CGRect rectToInvalidate;
	
	/* Response Data */
	NSData *imageData;
	float minValue;
	float maxValue;
	NSString *graphInfo;
	int refreshStage;		/* 1=XML 2=PDF */
	NSString *imageFile;
	NSMutableString *curXmlString;
}

@property (readonly) NSMutableArray *metrics;
@property (assign) LTEntity *metric;
@property (assign) CGSize size;
@property (copy) NSDate *referenceDate;
@property (copy) NSData *imageData;
@property (assign) float minValue;
@property (assign) float maxValue;
@property (assign) int startSec;
@property (assign) int endSec;
@property (assign) BOOL synchronous;
@property (assign) CGRect rectToInvalidate;

- (void) refresh;

@end
