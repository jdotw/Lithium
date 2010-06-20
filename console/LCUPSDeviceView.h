//
//  LCUPSDeviceView.h
//  Lithium Console
//
//  Created by James Wilson on 15/08/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCDeviceView.h"

@interface LCUPSDeviceView : LCDeviceView 
{
	/* Animation */
	float animationT;
	NSTimer *animationTimer;
	
	/* Bound values */
	NSString *mainsVoltage;
	NSString *mainsFrequency;
	NSString *batteryCapacity;
	NSString *batteryTime;
	NSString *batteryTemp;
	NSString *outputStatus;
	NSString *outputStatusInteger;
	NSString *outputVoltage;
	NSString *outputFrequency;
	NSString *outputLoad;
	NSString *outputCurrent;
}

#pragma mark "Constructors"
- (LCUPSDeviceView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "View Management"
- (void) removeFromSuperview;

#pragma mark "Geometry Methods"
- (float) visualWidth;
- (float) visualHeight;
- (NSRect) visualRect;
- (NSRect) mainsRect;
- (NSRect) batteryRect;
- (NSRect) outputRect;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

#pragma mark "Battery Drawing"
- (void) drawBattery;

#pragma mark "Mains Drawing"
- (void) drawMains;

#pragma mark "Output"
- (void) drawOutput;

#pragma mark "Output Icons"
- (void) drawBatteryIconInRect:(NSRect)rect;
- (void) drawOnMainsIconInRect:(NSRect)rect;
- (void) drawOnSmartTrimIconInRect:(NSRect)rect;
- (void) drawOnSmartBoostIconInRect:(NSRect)rect;

#pragma mark "Animation Timer"
- (void) nextFrame;

#pragma mark "Device"
- (LCEntity *) device;
- (void) setDevice:(LCEntity *)newDevice;

#pragma mark "Bound Accesors"
- (NSString *) mainsVoltage;
- (void) setMainsVoltage:(NSString *)string;
- (NSString *) mainsFrequency;
- (void) setMainsFrequency:(NSString *)string;
- (NSString *) batteryCapacity;
- (void) setBatteryCapacity:(NSString *)string;
- (NSString *) batteryTime;
- (void) setBatteryTime:(NSString *)string;
- (NSString *) batteryTemp;
- (void) setBatteryTemp:(NSString *)string;
- (NSString *) outputStatus;
- (void) setOutputStatus:(NSString *)string;
- (NSString *) outputStatusInteger;
- (void) setOutputStatusInteger:(NSString *)string;
- (NSString *) outputVoltage;
- (void) setOutputVoltage:(NSString *)string;
- (NSString *) outputFrequency;
- (void) setOutputFrequency:(NSString *)string;
- (NSString *) outputLoad;
- (void) setOutputLoad:(NSString *)string;
- (NSString *) outputCurrent;
- (void) setOutputCurrent:(NSString *)string;

@property float animationT;
@property (retain) NSTimer *animationTimer;
@property (retain,getter=mainsVoltage,setter=setMainsVoltage:) NSString *mainsVoltage;
@property (retain,getter=mainsFrequency,setter=setMainsFrequency:) NSString *mainsFrequency;
@property (retain,getter=batteryCapacity,setter=setBatteryCapacity:) NSString *batteryCapacity;
@property (retain,getter=batteryTime,setter=setBatteryTime:) NSString *batteryTime;
@property (retain,getter=batteryTemp,setter=setBatteryTemp:) NSString *batteryTemp;
@property (retain,getter=outputStatus,setter=setOutputStatus:) NSString *outputStatus;
@property (retain,getter=outputStatusInteger,setter=setOutputStatusInteger:) NSString *outputStatusInteger;
@property (retain,getter=outputVoltage,setter=setOutputVoltage:) NSString *outputVoltage;
@property (retain,getter=outputFrequency,setter=setOutputFrequency:) NSString *outputFrequency;
@property (retain,getter=outputLoad,setter=setOutputLoad:) NSString *outputLoad;
@property (retain,getter=outputCurrent,setter=setOutputCurrent:) NSString *outputCurrent;
@end
