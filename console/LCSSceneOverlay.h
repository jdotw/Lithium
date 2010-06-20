//
//  LCSSceneOverlay.h
//  Lithium Console
//
//  Created by James Wilson on 27/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCXMLObject.h"
#import "LCEntity.h"

@interface LCSSceneOverlay : LCXMLObject 
{
	NSString *uuid;

	NSRect frame;
	LCEntity *entity;
	BOOL autoSize;	
}

#pragma mark Constructors
+ (LCSSceneOverlay *) overlayWithFrame:(NSRect)initFrame;
- (LCSSceneOverlay *) initWithFrame:(NSRect)initFrame;

#pragma mark "XML Methods"
- (void) setXmlValuesUsingXmlNode:(LCXMLNode *)xmlNode;
- (NSXMLNode *) xmlNode;

#pragma mark Accessors
@property (assign) NSRect frame;
@property (retain) LCEntity *entity;
@property (assign) BOOL autoSize;
@property (copy) NSString *uuid;
+ (NSSize) minimumSize;

@end
