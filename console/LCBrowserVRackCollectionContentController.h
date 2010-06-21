//
//  LCBrowserVRackCollectionContentController.h
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "LCBrowser2Controller.h"
#import "LCBrowser2ContentViewController.h" 
#import "LCDocumentBreadcrumView.h"

@interface LCBrowserVRackCollectionContentController : LCBrowser2ContentViewController 
{
	/* Related Objects */
	LCBrowser2Controller *browser;
	
	/* Scenes */
	NSMutableArray *racks;
	NSIndexSet *selectedRackIndexes;
	
	/* UI ELements */
	IBOutlet LCDocumentBreadcrumView *crumView;	
}

#pragma mark "Properties"
@property (readonly) LCCustomerList *customerList;
@property (nonatomic, assign) LCBrowser2Controller *browser;
@property (readonly) NSMutableArray *racks;
@property (nonatomic,copy) NSIndexSet *selectedRackIndexes;

@end
