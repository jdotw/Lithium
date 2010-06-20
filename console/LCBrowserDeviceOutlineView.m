//
//  LCBrowserDeviceOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 20/04/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserDeviceOutlineView.h"

#import "LCEntity.h"
#import "LCViewCell.h"

@implementation LCBrowserDeviceOutlineView

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	[super awakeFromNib];

	/* Create viewControllerDictionary */
	viewControllerDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Set-up table view */
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[LCViewCell alloc] init] autorelease]];
}

#pragma mark "Reset views"

- (void) resetViews
{
	/* Remove all views */
	NSEnumerator *enumerator = [viewControllerDictionary objectEnumerator];
	id viewController;
	while (viewController = [enumerator nextObject])
	{
		[viewController removeViewAndContent];
	}
	[viewControllerDictionary removeAllObjects];
}

- (void) reloadData
{
	[self resetViews];
	[super reloadData];
}

#pragma mark "Outline Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	/* Get the object */
	id object = [item representedObject];
	if (object)
	{	
		/* Obtain an existing viewController */
		id viewController = [viewControllerDictionary objectForKey:[object description]];
		if (!viewController)
		{
			/* No existing, Obtain new controller */
			viewController = [object viewController];
			if (viewController)
			{ 
				[viewControllerDictionary setObject:viewController forKey:[object description]];
			}
		}
		
		/* Set hitView (to catch right-click, etc) */
		if ([[viewController view] respondsToSelector:@selector(setHitView:)])
		{ [[viewController view] setHitView:self]; }
		
		/* Set cell's sub-view */
		[(LCViewCell *)cell setSubView:[viewController view]];
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	if ([self levelForItem:item] == 0)
	{
		/* Device */
		return 38;
	}
	else
	{
		/* Incident */
		return 13;
	}
}

- (void)outlineViewItemWillExpand:(NSNotification *)notification
{
	[self resetViews];
}

- (void)outlineViewItemDidExpand:(NSNotification *)notification
{
}

- (void)outlineViewItemWillCollapse:(NSNotification *)notification
{
	[self resetViews];
}

- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
	
}

@synthesize viewColumn;
@synthesize viewControllerDictionary;
@end
