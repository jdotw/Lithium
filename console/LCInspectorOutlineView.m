//
//  LCInspectorOutlineView.m
//  Lithium Console
//
//  Created by James Wilson on 4/09/07.
//  Copyright 2007 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCInspectorOutlineView.h"
#import "LCInspectorViewCell.h"
#import "LCEntity.h"
#import "LCBrowserVerticalScroller.h"

@implementation LCInspectorOutlineView

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	/* Set-up table view */
	[super awakeFromNib];
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[LCInspectorViewCell alloc] init] autorelease]];
	[self setIndentationMarkerFollowsCell:NO];
	[self setIntercellSpacing:NSMakeSize(0,0)];
	
	/* Create visible views dict */
	visibleViews = [[NSMutableArray array] retain];
}

- (void) drawBackgroundInClipRect:(NSRect)clipRect
{
	[super drawBackgroundInClipRect:clipRect];
}

- (void) drawRect:(NSRect)rect
{
	[super drawRect:rect];
}

- (void) dealloc
{
	[visibleViews release];
	[super dealloc];
}

#pragma mark "Reset views"

- (void) resetViews
{
	/* Remove all views */
	NSView *view;
	for (view in visibleViews)
	{
		[view removeFromSuperview];
	}
	[visibleViews removeAllObjects];
	[self setNeedsDisplay:YES];
}

- (void) resetSubviewsOnly
{
	/* Remove all views */
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{ 
		if ([self levelForRow:i] == 1)
		{ 
			id item = [self itemAtRow:i];
			[[[item representedObject] view] removeFromSuperview];
		}
	}
	[self setNeedsDisplay:YES];
}

- (void) reloadData
{
	[self resetViews];
	[super reloadData];
}

#pragma mark "Outline Delegate methods"

- (void)outlineView:(NSOutlineView *)outlineView willDisplayOutlineCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
}

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	/* Get the object */
	id object = [item representedObject];
	if (object)
	{	
		/* Add to visible views */
		if (![visibleViews containsObject:[object view]]) 
		{
			if ([object view])
			{ 
				[visibleViews addObject:[object view]]; 
			}
		}
		
		/* Set hitView (to catch right-click, etc) */
		if ([[object view] respondsToSelector:@selector(setHitView:)])
		{ [[object view] performSelector:@selector(setHitView:) withObject:self]; }

		/* Check for last row */
		if ([self rowForItem:item] == [self numberOfRows] - 1)
		{ [(LCInspectorViewCell *)cell setLastRow:YES]; }		
		else
		{ [(LCInspectorViewCell *)cell setLastRow:NO]; }
		
		/* Check for header */
		if ([self levelForItem:item] == 0)
		{ [(LCInspectorViewCell *)cell setHeaderRow:YES]; }
		else
		{ [(LCInspectorViewCell *)cell setHeaderRow:NO]; }
		
		/* Set cell's sub-view */
		[(LCInspectorViewCell *)cell setSubView:[object view]];
//		[[object view] setNeedsDisplay:YES];
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	if ([[item representedObject] respondsToSelector:@selector(rowHeight)])
	{
		return [[item representedObject] rowHeight]; 
	}
	return 17.0;
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

#pragma mark "Highlight Methods"

- (BOOL) isOpaque
{ return NO; }

- (id) _highlightColorForCell:(NSCell *)cell
{ return nil; }

- (void) highlightSelectionInClipRect:(NSRect)clipRect
{ return; }


@synthesize viewColumn;
@synthesize visibleViews;
@end
