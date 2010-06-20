//
//  LCReportValue.h
//  Lithium Console
//
//  Created by Liam Elliott on 13/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCReportValue : NSObject {
	
	double value;
	double percent;
	double percentDelta;
	double valueDelta;
	
	NSString *units;
}


#pragma mark "Initialisation"


-(LCReportValue *)init;
-(LCReportValue *)initWithUnits:(NSString *)value;

-(void)dealloc;

#pragma mark "Accessors"
-(double) value;
-(void) setValue:(double)newValue;
-(NSString *)valueString;
-(NSString *)valueVolumeString;
-(double) valueDelta;
-(void) setValueDelta:(double)newValue;
-(NSString *)valueDeltaString;

-(double) percent;
-(void) setPercent:(double)newValue;
-(NSString *)percentString;
-(double) percentDelta;
-(void) setPercentDelta:(double)newValue;
-(NSString *)percentDeltaString;

-(NSString *)units;
-(void) setUnits:(NSString *)newValue;
@property (getter=value,setter=setValue:) double value;
@property (getter=percent,setter=setPercent:) double percent;
@property (getter=percentDelta,setter=setPercentDelta:) double percentDelta;
@property (getter=valueDelta,setter=setValueDelta:) double valueDelta;
@property (assign,getter=units,setter=setUnits:) NSString *units;
@end
