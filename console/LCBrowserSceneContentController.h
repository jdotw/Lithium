//
//  LCBrowserSceneContentController.h
//  Lithium Console
//
//  Created by James Wilson on 17/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2ContentViewController.h"
#import "LCBrowser2Controller.h"
#import "LCDocument.h"
#import "LCSSceneView.h"
#import "LCDocumentBreadcrumView.h"
#import "LCBrowserDocumentContentController.h"

@interface LCBrowserSceneContentController : LCBrowserDocumentContentController 
{
	/* UI Elements */
	IBOutlet LCSSceneView *sceneView;
	IBOutlet NSScrollView *scrollView;
	IBOutlet LCInspectorController *inspectorController;
	
	/* Selection */
	LCEntity *selectedEntity;
	LCSSceneOverlay *selectedOverlay;
}

#pragma mark "Constructors"
- (id) initWithDocument:(LCSSceneDocument *)initDocument inBrowser:(LCBrowser2Controller *)initBrowser;

#pragma mark "Selection"
@property (retain) LCEntity *selectedEntity;
@property (retain) LCSSceneOverlay *selectedOverlay;

@end
