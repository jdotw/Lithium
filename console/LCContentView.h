//
//  LCContentView.h
//  Lithium Console
//
//  Created by James Wilson on 24/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCContentView : NSView 
{
	NSImage *backImage;
}

- (NSImage *) backImage;
- (void) setBackImage:(NSImage *)value;

@property (retain,getter=backImage,setter=setBackImage:) NSImage *backImage;
@end
