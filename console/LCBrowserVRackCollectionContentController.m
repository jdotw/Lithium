//
//  LCBrowserVRackCollectionContentController.m
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserVRackCollectionContentController.h"

#import "LCDocumentBreadcrumItem.h"


@implementation LCBrowserVRackCollectionContentController

#pragma mark "Constructor"

- (id) initInBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [super initWithNibName:@"VRackCollectionContent" bundle:nil];
	if (self)
	{
		/* Create Rack List */
		racks = [[NSMutableArray array] retain];
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			for (LCDocument *doc in [customer.documentList documents])
			{ 
				if ([doc.type isEqualToString:@"vrack"])
				{
					[racks addObject:doc];
					[doc getDocument]; 
				}
			}
		}
		
		/* Set Controller Properties */
		self.resizeMode = RESIZE_TOP;
		self.browser = initBrowser;
		
		/* Load NIB */
		[self loadView];		
		
		/* Setup breadcrum view */
		crumView.delegate = self;
		LCDocumentBreadcrumItem *item = [LCDocumentBreadcrumItem new];
		item.title = @"Virtual Racks";
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
	if ([selectedRackIndexes firstIndex] != NSNotFound)
	{ [browser.browserTreeOutlineView selectVRackDocument:[racks objectAtIndex:[selectedRackIndexes firstIndex]]]; }
}

#pragma mark "Breadcrum Methods"

- (IBAction) breadcrumClicked:(id)sender
{

}

#pragma mark "Properties"

- (LCCustomerList *) customerList
{ return [LCCustomerList masterList]; }

@synthesize browser;
@synthesize racks;
@synthesize selectedRackIndexes;


@end
