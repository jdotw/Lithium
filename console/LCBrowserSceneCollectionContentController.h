//
//  LCBrowserSceneCollectionContentController.h
//  Lithium Console
//
//  Created by James Wilson on 18/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h" 
#import "LCDocumentBreadcrumView.h"
#import "LCSSceneCollectionView.h"

@interface LCBrowserSceneCollectionContentController : LCBrowser2ContentViewController 
{
	/* Related Objects */
	LCBrowser2Controller *browser;
	
	/* Scenes */
	NSMutableArray *scenes;
	NSIndexSet *selectedSceneIndexes;
	
	/* UI ELements */
	IBOutlet LCDocumentBreadcrumView *crumView;
	IBOutlet LCSSceneCollectionView *collectionView;
	IBOutlet NSScrollView *scrollView;
}

#pragma mark "Properties"
@property (readonly) LCCustomerList *customerList;
@property (assign) LCBrowser2Controller *browser;
@property (readonly) NSMutableArray *scenes;
@property (copy) NSIndexSet *selectedSceneIndexes;

@end
