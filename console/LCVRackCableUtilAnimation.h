//
//  LCVRackCableUtilAnimation.h
//  Lithium Console
//
//  Created by James Wilson on 18/06/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCVRackCableUtilAnimation : NSAnimation 
{
	id cable;
}

- (id) cable;
- (void) setCable:(id)newCable;

@property (assign,getter=cable,setter=setCable:) id cable;
@end
