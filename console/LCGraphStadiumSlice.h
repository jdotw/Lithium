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

@property (assign) LCGraphStadiumController *stadiumController;

@property (assign) CALayer *sliceLayer;
@property (assign) LCGraphStadiumSliceLayer *imageLayer;
@property (assign) CALayer *reflectionLayer;
@property (assign) CALayer *gradientLayer;

@property (assign) int index;
@property (assign) int total;
@property (assign) int rows;

@property (readonly) NSMutableArray *labelLayers;

@end
