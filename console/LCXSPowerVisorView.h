//
//  LCXSPowerVisorView.h
//  Lithium Console
//
//  Created by James Wilson on 12/03/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCXSPowerVisorView : NSView 
{

}

- (LCXSPowerVisorView *) initWithFrame:(NSRect)frame;
- (void) drawRect:(NSRect)rect;
- (void) fadeIn;
- (void) fadeOut;

@end
