//
//  LCXSAirFlowView.h
//  Lithium Console
//
//  Created by James Wilson on 19/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCEntity.h"

@interface LCXSAirFlowView : NSView
{
	LCEntity *device;
}

@property (nonatomic,retain) LCEntity *device;
@end
