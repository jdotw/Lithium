//
//  LCViewTableView.m
//  Lithium Console
//
//  Created by James Wilson on 10/01/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCViewTableView.h"
#import "LCViewCell.h"
#import "LCObjectView.h"
#import "LCEntity.h"

@implementation LCViewTableView

#pragma mark "NIB Initialisation"

- (void) awakeFromNib
{
	/* Create viewControllerDictionary */
	viewControllerDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Set-up table view */
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[LCViewCell alloc] init] autorelease]];
}

#pragma mark "Reload method"

- (void) reloadData
{
	/* Remove all views */
	NSArray *values = [viewControllerDictionary allValues];
	for (id viewController in values)
	{
		[viewController performSelector:@selector(removeViewAndContent)];
	}
	[viewControllerDictionary removeAllObjects];
	
	/* Call super-class reload */
	[super reloadData];
}

#pragma mark "Delegate methods"

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if (tableColumn == viewColumn)
	{
		/* Get the object */
		id object = [[tableContentController arrangedObjects] objectAtIndex:rowIndex];
		if (object)
		{
			/* Obtain an existing viewController */
			id viewController = [viewControllerDictionary objectForKey:[object description]];
			if (!viewController)
			{
				/* No existing, Obtain new controller */
				viewController = [(LCEntity *) object viewController];
				if (viewController)
				{ 
					[viewControllerDictionary setObject:viewController forKey:[object description]];
				}
			}

			/* Set hitView (to catch right-click, etc) */
			if ([[viewController view] respondsToSelector:@selector(setHitView:)])
			{ [(LCObjectView *) [viewController view] setHitView:self]; }
			
			/* Set cell's sub-view */
			[(LCViewCell *)cell setSubView:[viewController view]];
		}
	}
}

@synthesize viewColumn;
@synthesize viewControllerDictionary;
@end
