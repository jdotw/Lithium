//
//  LCSSceneWindowController.h
//  Lithium Console
//
//  Created by James Wilson on 28/08/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import <Cocoa/Cocoa.h>

#import "LCSSceneView.h"

@interface LCSSceneWindowController : NSWindowController 
{
	id sceneDocument;
	IBOutlet LCSSceneView *sceneView;
}

#pragma mark "Constructors"
- (LCSSceneWindowController *) initWithSceneDocument:(id)initDocument;
- (void) dealloc;
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "Window Delegates"
- (void) windowWillClose:(NSNotification *)notification;

#pragma mark "UI Actions"
- (IBAction) setBackgroundImageClicked:(id)sender;


@property (nonatomic,retain) id sceneDocument;
@property (nonatomic,retain) LCSSceneView *sceneView;
@end
