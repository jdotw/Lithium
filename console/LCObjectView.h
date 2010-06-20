//
//  LCObjectView.h
//  Lithium Console
//
//  Created by James Wilson on 26/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCObjectView : NSView 
{
	NSView *hitView;
}

#pragma mark "Accessor Methods"

- (NSView *) hitView;
- (void) setHitView:(NSView *)view;

@property (assign,getter=hitView,setter=setHitView:) NSView *hitView;
@end
