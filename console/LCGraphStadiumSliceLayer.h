//
//  LCGraphStadiumSliceLayer.h
//  Lithium Console
//
//  Created by James Wilson on 2/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <QuartzCore/QuartzCore.h>

#import "LCGraphStadiumController.h"

@interface LCGraphStadiumSliceLayer : CALayer 
{
	LCGraphStadiumController *controller;
}

@property (assign) LCGraphStadiumController *controller;

@end
