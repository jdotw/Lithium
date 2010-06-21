//
//  LCSSceneDocument.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>
#import "LCSSceneOverlay.h"
#import "LCDocument.h"

@interface LCSSceneDocument : LCDocument 
{
	/* Background */
	NSImage *backgroundImage;
	
	/* Overlays */
	NSMutableArray *overlays;
	NSMutableDictionary *overlayDictionary;
}

#pragma mark "Properties"
@property (nonatomic,copy) NSImage *backgroundImage;
@property (readonly) NSMutableArray *overlays;
- (void) insertObject:(LCSSceneOverlay *)overlay inOverlaysAtIndex:(unsigned int)index;
- (void) removeObjectFromOverlaysAtIndex:(unsigned int)index;
@property (readonly) NSMutableDictionary *overlayDictionary;

@end
