//
//  LCBrowserSceneCollectionContentController.m
//  Lithium Console
//
//  Created by James Wilson on 18/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserSceneCollectionContentController.h"
#import "LCSSceneDocument.h"
#import "LCDocumentBreadcrumItem.h"

@implementation LCBrowserSceneCollectionContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"SceneCollectionContent" bundle:nil];
	if (self)
	{
		/* Create Scene List */
		scenes = [[NSMutableArray array] retain];
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			for (LCSSceneDocument *doc in [customer.documentList documents])
			{ 
				if ([doc.type isEqualToString:@"scene"]) 
				{
					[scenes addObject:doc];
					[doc getDocument]; 
				}
			}
		}

		/* Set Controller Properties */
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		
		/* Load NIB */
		[self loadView];	
		[scrollView setBackgroundColor:[NSColor clearColor]];

		/* Setup breadcrum view */
		crumView.delegate = self;
		LCDocumentBreadcrumItem *item = [LCDocumentBreadcrumItem new];
		item.title = @"State Scenes";
		[crumView insertObject:item inItemsAtIndex:0];
		[item autorelease];		
	}
	return self;
}	

- (void) dealloc
{
	[super dealloc];
}

#pragma mark "UI Actions"

- (void) collectionViewDoubleClicked:(id)sender
{
	if ([selectedSceneIndexes firstIndex] != NSNotFound)
	{ [browser.browserTreeOutlineView selectSceneDocument:[scenes objectAtIndex:[selectedSceneIndexes firstIndex]]]; }
}

#pragma mark "Breadcrum Methods"

- (IBAction) breadcrumClicked:(id)sender
{
	
}

#pragma mark "Properties"

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@synthesize browser;
@synthesize scenes;
@synthesize selectedSceneIndexes;

@end
