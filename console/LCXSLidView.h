//
//  LCXSLidView.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCXSLidView : NSView 
{

}

- (LCXSLidView *) initWithFrame:(NSRect)frame;
- (void) drawRect:(NSRect)rect;
- (void) fadeOut;

@end
