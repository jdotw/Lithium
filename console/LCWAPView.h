//
//  LCWAPView.h
//  Lithium Console
//
//  Created by James Wilson on 7/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"
#import "LCWAPViewClient.h"
#import "LCDeviceView.h"

@interface LCWAPView : LCDeviceView
{
	/* Timer */
	NSTimer *animationTimer;
	
	/* Clients */
	NSMutableDictionary *clientDictionary;
	
	/* Sweep */
	float sweepAngle;
}

#pragma mark "Constructors"
- (LCWAPView *) initWithDevice:(LCEntity *)initDevice inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "View Management"
- (void) removeFromSuperview;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

#pragma mark "Throughput Indicators"
- (void) drawThroughputIndicatorsForClient:(LCWAPViewClient *)client 
									 speed:(NSString *)speed 
									   fps:(NSString *)fps 
						   speedIndicatorT:(float)speedIndicatorT
								   xOffset:(float)xOffset 
								   yOffset:(float)yOffset
								cableWidth:(float)cableWidth;

#pragma mark "KVO Methods"
- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context;


@property (nonatomic,retain) NSTimer *animationTimer;
@property (nonatomic,retain) NSMutableDictionary *clientDictionary;
@property float sweepAngle;
@end
