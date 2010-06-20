//
//  LCBackgroundView.h
//  Lithium Console
//
//  Created by James Wilson on 21/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>


@interface LCBackgroundView : NSView 
{
	NSImage *image;
}

#pragma mark "Drawing Method"
- (void) drawRect:(NSRect)rects;

#pragma mark "Accessors"
- (NSImage *) image;
- (void) setImage:(NSImage *)newImage;

@property (retain,getter=image,setter=setImage:) NSImage *image;
@end

