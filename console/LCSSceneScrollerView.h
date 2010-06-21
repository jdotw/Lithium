//
//  LCSSceneScrollerView.h
//  Lithium Console
//
//  Created by James Wilson on 1/09/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCSSceneOverlay.h"

@interface LCSSceneScrollerView : NSView 
{
	LCSSceneOverlay *overlay;
}

#pragma mark "Constructors"
- (LCSSceneScrollerView *) initWithOverlay:(LCSSceneOverlay *)initOverlay inFrame:(NSRect)frame;
- (void) removeViewAndContent;
- (void) dealloc;

#pragma mark "Drawing Method"
- (void) drawRect:(NSRect)rect;

#pragma mark "Hit Test"
- (NSView *)hitTest:(NSPoint)aPoint;

@property (nonatomic,retain) LCSSceneOverlay *overlay;
@end
