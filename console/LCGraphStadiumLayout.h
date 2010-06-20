//
//  LCGraphStadiumLayout.h
//  Lithium Console
//
//  Created by James Wilson on 29/04/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <QuartzCore/QuartzCore.h>

#import "LCGraphStadiumController.h"

@interface LCGraphStadiumLayout : NSObject 
{
	LCGraphStadiumController *controller;
}

@property (assign) LCGraphStadiumController *controller;

@end
