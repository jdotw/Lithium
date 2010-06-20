//
//  LCBrowserSearchContentController.m
//  Lithium Console
//
//  Created by James Wilson on 24/08/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserSearchContentController.h"
#import "LCCustomerList.h"
#import "LCBrowser2Controller.h"

@implementation LCBrowserSearchContentController

#pragma mark "Constructors"

- (id) initInBrowser:(id)initBrowser
{
	self = [super initWithNibName:@"SearchContent" bundle:nil];
	if (self != nil) 
	{
		/* Set/Create Properties */
		self.browser = initBrowser;
		
		/* Create sources */
		sources = [[NSMutableArray array] retain];
		LCSearchSourceItem *source = [[[LCSearchSourceItem alloc] initWithCustomer:nil] autorelease];
		self.selectedSource = source;
		[self insertObject:source inSourcesAtIndex:sources.count];
		for (LCCustomer *customer in [LCCustomerList masterArray])
		{
			source = [[[LCSearchSourceItem alloc] initWithCustomer:customer] autorelease];
			[self insertObject:source inSourcesAtIndex:sources.count];
		}

		/* Set up views */
		[self loadView];
		[inspectorController bind:@"target" 
						 toObject:self
					  withKeyPath:@"selectedEntity"
						  options:nil];
		[resultArrayController addObserver:self
								forKeyPath:@"selectedObjects"
								   options:NSKeyValueObservingOptionOld|NSKeyValueObservingOptionNew
								   context:nil];
	}
	return self;
}

- (void) removedFromBrowserWindow
{
	[inspectorController unbind:@"target"];
	[resultArrayController removeObserver:self forKeyPath:@"selectedObjects"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[searchString release];
	[sources release];
	[selectedSource release];
	[selectedEntity release];
	[selectedEntities release];
	[super dealloc];
}

#pragma mark "KVO Methods"

- (void) observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if (object == resultArrayController)
	{
		self.selectedEntities = [resultArrayController selectedObjects];
	}
}

#pragma mark "Sources"

@synthesize sources;
- (void) insertObject:(LCSearchSourceItem *)item inSourcesAtIndex:(unsigned int)index
{
	[sources insertObject:item atIndex:index];
}
- (void) removeObjectFromSourcesAtIndex:(unsigned int)index
{
	[sources removeObjectAtIndex:index];	
}

#pragma mark "Entity Selection"

@synthesize selectedEntity;
@synthesize selectedEntities;
- (void) setSelectedEntities:(NSArray *)value
{
	[selectedEntities release];
	selectedEntities = [value copy];
	
	if (selectedEntities.count > 0)
	{ self.selectedEntity = [selectedEntities objectAtIndex:0]; }
	else
	{ self.selectedEntity = nil; }
}

- (void) resultTableViewDoubleClicked:(NSArray *)selectedObjects
{
	if (selectedObjects.count > 0)
	{ [(LCBrowser2Controller *)browser selectEntity:[selectedObjects objectAtIndex:0]]; }
}

#pragma mark "Properties"

@synthesize browser;
@synthesize searchString;
- (void) setSearchString:(NSString *)value
{
	[searchString release];
	searchString = [value copy];
	
	for (LCSearchSourceItem *source in sources)
	{ source.searchString = searchString; }
	
	[selectedSource search];
}

- (CGFloat) preferredFixedComponentHeight
{ return 120.0; }

@synthesize selectedSource;

@end
