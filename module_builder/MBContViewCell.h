//
//  MBContViewCell.h
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MBContViewCell : NSTextFieldCell 
{
	NSView *subView;
	NSView *prefsView;
	NSView *customView;
	int level;
	BOOL selected;
}

#pragma mark Initialisation 
- (void) dealloc;

#pragma mark View Manipulation
@property (assign) NSView *subView;
@property (assign) NSView *prefsView;
@property (assign) NSView *customView;

#pragma mark Drawing Methods
- (void) drawWithFrame:(NSRect) cellFrame inView:(NSView *) controlView;

#pragma mark Accessors
- (int) level;
- (void) setLevel:(int)num;
@property (assign) BOOL selected;

@end
