//
//  MBContOutlineView.m
//  ModuleBuilder
//
//  Created by James Wilson on 12/02/08.
//  Copyright 2008 LithiumCorp Pty Ltd. All rights reserved.
//

#import "MBContOutlineView.h"
#import "MBContViewCell.h"
#import "MBContainer.h"
#import "MBMetric.h"
#import "ModuleDocument.h"
#import "MBOutlineViewController.h"
#import "LCRoundedBezierPath.h"

#import "MBRateMetric.h"

@implementation MBContOutlineView

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	/* Set-up table view */
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[MBContViewCell alloc] init] autorelease]];
	[self setIndentationMarkerFollowsCell:YES];
	[self setIntercellSpacing:NSMakeSize(0 , 2.0)];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(reloadData) name:@"ResetItYo" object:nil];
	
	/* Create visible views dict */
	viewControllers = [[NSMutableDictionary dictionary] retain];
}

#pragma mark "Reset views"

- (void) resetViews
{
	NSArray *controllerArray = [viewControllers allValues];
	for (MBOutlineViewController *controller in controllerArray)
	{ 
		[[controller view] removeFromSuperviewWithoutNeedingDisplay]; 
		[[controller prefsView] removeFromSuperviewWithoutNeedingDisplay];
		[[controller customView] removeFromSuperviewWithoutNeedingDisplay];
	}
	[self setNeedsDisplay:YES];
}

- (void) reloadData
{
	[self resetViews];
	[super reloadData];
}

#pragma mark "Selection"

- (void) selectTrigger:(MBTrigger *)trigger
{
	/* Select the new trigger */
	MBMetric *metric = (MBMetric *) [trigger parent];
	MBContainer *container = (MBContainer *) [metric parent];
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{ 
		id item = [self itemAtRow:i];
		MBEntity *entity = [item representedObject];
		
		if (entity == container || entity == metric)
		{ [self expandItem:item]; }
		else if (entity == trigger)
		{ 
			[self selectRow:i byExtendingSelection:NO]; 
			[self scrollRowToVisible:i];
			return;
		}
	}	
}

- (void) selectMetric:(MBMetric *)metric
{
	/* Select the new trigger */
	MBContainer *container = (MBContainer *) [metric parent];
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{ 
		id item = [self itemAtRow:i];
		MBEntity *entity = [item representedObject];
		
		if (entity == container)
		{ [self expandItem:item]; }
		else if (entity == metric)
		{ 
			[self selectRow:i byExtendingSelection:NO]; 
			[self scrollRowToVisible:i];
			return;
		}
	}	
}

- (void) selectContainer:(MBContainer *)container
{
	/* Select the new trigger */
	int i;
	for (i=0; i < [self numberOfRows]; i++)
	{ 
		id item = [self itemAtRow:i];
		MBEntity *entity = [item representedObject];
		
		if (entity == container)
		{ 
			[self selectRow:i byExtendingSelection:NO]; 
			[self scrollRowToVisible:i];
			return;
		}
	}	
}

- (void) deleteSelection
{
	NSEnumerator *selectedEnum = [[treeController selectedObjects] objectEnumerator];
	MBEntity *entity;
	while (entity=[selectedEnum nextObject])
	{
		if ([[entity type] intValue] > 4)
		{ [[entity parent] removeObjectFromChildrenAtIndex:[[[entity parent] children] indexOfObject:entity]]; }
		else
		{ 
			MBContainer *container = (MBContainer *) entity;
			ModuleDocument *document = (ModuleDocument *) [container document];
			[document removeObjectFromContainersAtIndex:[[document containers] indexOfObject:container]];
		}
	}	
	[self reloadData];
}

- (IBAction) delete:(id)sender
{
	[self deleteSelection];
}

#pragma mark "Outline Delegate methods"

- (MBOutlineViewController *) viewControllerForEntity:(MBEntity *)entity
{
	MBOutlineViewController *viewController = [viewControllers objectForKey:[entity description]];
	if (!viewController) 
	{
		viewController = (MBOutlineViewController *) [entity viewController];
		if (viewController)	[viewControllers setObject:viewController forKey:[entity description]];
	}
	return viewController;
}

- (void)outlineView:(NSOutlineView *)outlineView willDisplayOutlineCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
}

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	/* Get the entity (container or metric) */
	MBEntity *entity = [item representedObject];
	if (!entity) return;
	
	/* Find a view controller (existing or new) */
	MBOutlineViewController *viewController = [self viewControllerForEntity:entity];
		
	/* Set cell's sub-view */
	[(MBContViewCell *)cell setLevel:[self levelForItem:item]];
	[(MBContViewCell *)cell setSubView:[viewController view]];
	if ([self isRowSelected:[self rowForItem:item]]) [(MBContViewCell *)cell setSelected:YES];
	else [(MBContViewCell *)cell setSelected:NO];
	if ([entity infoViewVisible])
	{
		[(MBContViewCell *)cell setPrefsView:[viewController prefsView]];
		[(MBContViewCell *)cell setCustomView:[viewController customView]];
	}
	else
	{
		[(MBContViewCell *)cell setPrefsView:nil];
		[(MBContViewCell *)cell setCustomView:nil];
	}
}

- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item
{
	MBEntity *entity = (MBEntity *) [item representedObject];
	MBOutlineViewController *viewController = [self viewControllerForEntity:entity];
	if (viewController)
	{
		/* Ask viewController for size */
		return [viewController rowHeight];
	}
	else
	{
		return [entity rowHeight];	
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

#pragma mark "Drag Image Handling"

- (NSImage *)dragImageForRowsWithIndexes:(NSIndexSet *)dragRows 
							tableColumns:(NSArray *)tableColumns
								   event:(NSEvent *)dragEvent 
								  offset:(NSPointPointer)dragImageOffset
{
	/* Create image */
	NSViewController *viewController = [self viewControllerForEntity:[[self itemAtRow:[dragRows firstIndex]] representedObject]];
	NSImage *viewImage = [[NSImage alloc] initWithData:[[viewController view] dataWithPDFInsideRect:[[viewController view] bounds]]];
	NSImage *dragImage = [[NSImage alloc] initWithSize:NSMakeSize([viewImage size].width+8, [viewImage size].height+8)];
	[dragImage lockFocus];
	NSBezierPath *path = [LCRoundedBezierPath pathInRect:NSMakeRect(0,0,[dragImage size].width,[dragImage size].height)];
	[[NSColor colorWithCalibratedWhite:0.2 alpha:0.6] setFill];
	[path fill];
	[viewImage drawInRect:NSMakeRect(4,4,[viewImage size].width, [viewImage size].height) 
				 fromRect:NSMakeRect(0,0,[viewImage size].width, [viewImage size].height) 
				operation:NSCompositeSourceOver
				 fraction:1.0];
	[dragImage unlockFocus];
	
	return dragImage;
}



#pragma mark "Key Down Methods"

- (void)keyDown:(NSEvent *)event
{
	unichar key = [[event charactersIgnoringModifiers] characterAtIndex:0];
	unsigned int flags = ( [event modifierFlags] & 0x00FF );
	
	if (((key == NSDeleteCharacter) || (key == NSBackspaceCharacter)) && (flags == 0))
	{
		/* Delete selection */
		[self deleteSelection];
	}
	else
	{
		[super keyDown:event];
	}
}	

@end
