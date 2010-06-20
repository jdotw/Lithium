//
//  LCBrowserTreeDocuments.m
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCBrowserTreeDocuments.h"

#import "LCCustomer.h"
#import "LCCustomerList.h"

@interface LCBrowserTreeDocuments (private)

- (void) observeCustomer:(LCCustomer *)customer;
- (void) unobserveCustomer:(LCCustomer *)customer;
- (void) rebuildDocumentsList;

@end

@implementation LCBrowserTreeDocuments

#pragma mark "Constructors"

- (LCBrowserTreeDocuments *) init
{
	[super init];
	
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{ 
		[self observeCustomer:customer]; 
		[customer.documentList highPriorityRefresh];
	}
	
	/* Observe change in customers */
	[[LCCustomerList masterList] addObserver:self 
								  forKeyPath:@"array"
									 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
									 context:NULL];	
	
	[self rebuildDocumentsList];
	self.isBrowserTreeLeaf = NO;
	
	return self;
}

- (void) dealloc
{
	[[LCCustomerList masterList] removeObserver:self forKeyPath:@"array"];
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{ [self unobserveCustomer:customer]; }
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	/* Change in the customers documentr list */
	if ([keyPath isEqualToString:@"documents"])
	{
		[self rebuildDocumentsList];
	}
	
	/* Change in the customer list */
	if ([keyPath isEqualToString:@"array"])
	{
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeNewKey])
			{ [self observeCustomer:customer]; }
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (LCCustomer *customer in [change objectForKey:NSKeyValueChangeOldKey])
			{ [self unobserveCustomer:customer]; }
		}
	}
}

- (void) observeCustomer:(LCCustomer *)customer
{
	[customer.documentList addObserver:self 
							forKeyPath:@"documents" 
							   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)  
							   context:NULL];
}

- (void) unobserveCustomer:(LCCustomer *)customer
{
	[customer.documentList removeObserver:self forKeyPath:@"documents"];
}

#pragma mark "Child Array Management"

- (void) rebuildDocumentsList
{
	NSMutableArray *array = [NSMutableArray array];
	for (LCCustomer *customer in [[LCCustomerList masterList] array])
	{
		for (LCDocument *document in customer.documentList.documents)
		{
			if ([document.type isEqualToString:[self documentType]])
			{ [array addObject:document]; }
		}
	}
	self.children = array;
}

#pragma mark "Tree Item Methods"

- (NSString *) displayString
{
	return @"Documents";
}

- (NSImage *) treeIcon
{
	return [NSImage imageNamed:@"documents_16.tif"];
}

- (BOOL) selectable
{ return YES; }

- (NSString *) documentType
{ return nil; }

@end
