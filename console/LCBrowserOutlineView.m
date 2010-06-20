//
//  LCBrowserOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 8/12/05.
//  Copyright 2005 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserOutlineView.h"
#import "LCViewCell.h"
#import "LCEntity.h"
#import "LCEntityViewController.h"
#import "LCXMLRequest.h"

@implementation LCBrowserOutlineView

#pragma mark "Initialisation"

- (void) awakeFromNib
{
	/* Create entityViewControllers dictionary */
	entityViewControllers = [[NSMutableDictionary dictionary] retain];
	
	/* Set-up outline view */
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[LCViewCell alloc] init] autorelease]];
}

#pragma mark "Reload Method"

- (void) reloadData
{
	/* Remove all entityViews */
	NSArray *viewControllerArray = [entityViewControllers allValues];
	for (LCEntityViewController *viewController in viewControllerArray)
	{
		[viewController removeViewAndContent];
	}
	[entityViewControllers removeAllObjects];

    /* Call super-class reloadData */
    [super reloadData];
}

#pragma mark "Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	if (tableColumn == viewColumn)
	{
		/* Retrieve entity */
		LCEntity *entity = [item representedObject];
		
		/* Obtain existing entityViewController */
		LCEntityViewController *viewController = [entityViewControllers objectForKey:[[entity entityAddress] addressString]];
		if (!viewController)
		{
			/* New existing entityViewController, obtain a new one */
			viewController = [entity entityViewController];
			if (viewController)
			{ [entityViewControllers setObject:viewController forKey:[[entity entityAddress] addressString]]; }
		}
	
		/* Set cell's sub-view */
		[(LCViewCell *)cell setSubView:[viewController view]];		
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	float rowHeight = 12;
	LCEntity *entity = [item representedObject];
	
	/* Obtain existing entityViewController */
	LCEntityViewController *viewController = [entityViewControllers objectForKey:[[entity entityAddress] addressString]];
	if (!viewController)
	{
		/* New existing entityViewController, obtain a new one */
		viewController = [entity entityViewController];
		if (viewController)
		{ [entityViewControllers setObject:viewController forKey:[[entity entityAddress] addressString]]; }
	}
	
	/* Get size */
	if (viewController && [viewController view])
	{ rowHeight = [[viewController view] bounds].size.height; }
	
	return rowHeight;
}

- (void)outlineViewItemWillExpand:(NSNotification *)notification
{
	/* Remove all entityViews */
	NSArray *viewControllerArray = [entityViewControllers allValues];
	for (LCEntityViewController *viewController in viewControllerArray)
	{
		[viewController removeViewAndContent];
	}
	[entityViewControllers removeAllObjects];	
}

- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
	[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(reloadData) userInfo:nil repeats:NO];
}

- (void)outlineViewItemWillCollapse:(NSNotification *)notification
{
	/* Remove all entityViews */
	NSArray *viewControllerArray = [entityViewControllers allValues];
	for (LCEntityViewController *viewController in viewControllerArray)
	{
		[viewController removeViewAndContent];
	}
	[entityViewControllers removeAllObjects];	
}

- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
	[NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(reloadData) userInfo:nil repeats:NO];
}

#pragma mark "Menu Event Handling"

- (NSMenu *) menuForEvent:(NSEvent *)event
{
	NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:point];
	int col = [self columnAtPoint:point];
	
	if (row == -1 || col == -1) return nil;
	
	[self selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
	
	return [self menu];
}

#pragma mark "Drag Image Handling"

- (NSImage *)dragImageForRowsWithIndexes:(NSIndexSet *)dragRows 
							tableColumns:(NSArray *)tableColumns
								   event:(NSEvent *)dragEvent 
								  offset:(NSPointPointer)dragImageOffset
{
	/* Retrieve entity */
	id item = [self itemAtRow:[dragRows firstIndex]];
	LCEntity *entity = [item representedObject];
	
	/* Create image */
	LCEntityViewController *viewController = [entity entityViewController];
	NSRect r = [[viewController view] bounds];
	NSImage *dragImage = [[NSImage alloc] initWithData:[[viewController view] dataWithPDFInsideRect:r]];
	
	return [dragImage autorelease];
}

@synthesize treeController;
@synthesize viewColumn;
@synthesize entityViewControllers;
@end
