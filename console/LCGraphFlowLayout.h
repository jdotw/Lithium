//
//  LCGraphFlowLayout.h
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCGraphFlowController.h"

@interface LCGraphFlowLayout : NSObject 
{
	LCGraphFlowController *controller;
}

@property (assign) LCGraphFlowController *controller;

@end
