//
//  LCXSBlowerView.h
//  Lithium Console
//
//  Created by James Wilson on 4/02/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCEntity.h"

@interface LCXSBlowerView : NSView 
{
	/* Object */
	LCEntity *blowerObject;
	
	/* Images */
	NSImage *blowerImage;
}


#pragma mark "Constructors"
- (LCXSBlowerView *) initWithBlower:(LCEntity *)initBlower inFrame:(NSRect)frame;
- (void) dealloc;

#pragma mark "View Management"
- (void) removeFromSuperview;

#pragma mark "Drawing"
- (void) drawRect:(NSRect)rect;

@property (nonatomic,retain) LCEntity *blowerObject;
@property (nonatomic,retain) NSImage *blowerImage;
@end
