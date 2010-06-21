//
//  LCGraphStadiumSlice.h
//  Lithium Console
//
//  Created by James Wilson on 29/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCMetricGraphController.h"
#import "LCGraphStadiumController.h"
#import "LCGraphStadiumSliceLayer.h"

@interface LCGraphStadiumSlice : NSObject 
{
	int index;
	int total;
	int rows;
	
	NSMutableArray *graphControllers;
	LCGraphStadiumController *stadiumController;
	
	CALayer *sliceLayer;
	LCGraphStadiumSliceLayer *imageLayer;
	CALayer *reflectionLayer;
	CALayer *gradientLayer;
	
	NSMutableArray *labelLayers;
}

@property (readonly) NSMutableArray *graphControllers;
- (void) insertObject:(LCMetricGraphController *)controller inGraphControllersAtIndex:(unsigned int)index;
- (void) removeObjectFromGraphControllersAtIndex:(unsigned int)index;

@property (nonatomic, assign) LCGraphStadiumController *stadiumController;

@property (nonatomic, assign) CALayer *sliceLayer;
@property (nonatomic, assign) LCGraphStadiumSliceLayer *imageLayer;
@property (nonatomic, assign) CALayer *reflectionLayer;
@property (nonatomic, assign) CALayer *gradientLayer;

@property (nonatomic, assign) int index;
@property (nonatomic, assign) int total;
@property (nonatomic, assign) int rows;

@property (readonly) NSMutableArray *labelLayers;

@end
