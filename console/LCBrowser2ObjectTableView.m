//
//  LCBrowser2ObjectTableView.m
//  Lithium Console
//
//  Created by James Wilson on 14/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowser2ObjectTableView.h"
#import "LCBrowserVerticalScroller.h"
#import "LCEntityViewController.h"
#import "LCViewCell.h"
#import "LCObjectView.h"
#import "LCRoundedBezierPath.h"

@implementation LCBrowser2ObjectTableView

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	/* Awake super class */
	[super awakeFromNib];
	
	/* Add container content observer */
	[containersArrayController addObserver:self
								forKeyPath:@"selection" 
								   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
								   context:NULL];
	
	/* Create row height dict */
	rowHeightDict = [[NSMutableDictionary dictionary] retain];

	/* Create viewControllerDictionary */
	viewControllerDictionary = [[NSMutableDictionary dictionary] retain];
	
	/* Set-up table view */
	[self setDelegate:self];
	viewColumn = [[self tableColumns] objectAtIndex:0];
	[viewColumn setDataCell:[[[LCViewCell alloc] init] autorelease]];
	
	[self setIntercellSpacing:NSMakeSize(3.0, 0.0)];
}

- (void) dealloc
{
	[containersArrayController removeObserver:self forKeyPath:@"selection"];
	[super dealloc];
}

#pragma mark "Reload method"

- (void) reloadData
{
	/* Remove all views */
	NSEnumerator *enumerator = [viewControllerDictionary objectEnumerator];
	id viewController;
	while (viewController = [enumerator nextObject])
	{
		[viewController removeViewAndContent];
	}
	[viewControllerDictionary removeAllObjects];
	
	/* Call super-class reload */
	[super reloadData];
}

#pragma mark "Selection Management"

- (void) selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend
{
	[super selectRowIndexes:indexes byExtendingSelection:extend];
	[self setNeedsDisplay:YES];
}

- (void)deselectRow:(NSInteger)row;
{
	[super deselectRow:row];
	[self setNeedsDisplay:YES]; 
}

#pragma mark "KVO"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqual:@"selection"])
	{
		/* Clear cache of row heights */
		[rowHeightDict removeAllObjects];
		
		/* Selection changed */
		[self reloadData];
	}
}

#pragma mark "Row Height"

- (float)tableView:(NSTableView *)tableView heightOfRow:(int)rowIndex
{
	float height = 13.0;

	/* Retrieve object and controller */
	id object = [[tableContentController arrangedObjects] objectAtIndex:rowIndex];
	id viewController = [viewControllerDictionary objectForKey:[object description]];
	
	/* Get from cache */
	NSNumber *heightNumber = [rowHeightDict objectForKey:[NSNumber numberWithInt:rowIndex]];
	if (heightNumber)
	{
		/* Cached value! */
		return [heightNumber floatValue];
	}
	else
	{
		/* No cached value */
		/* Get the object */
		if (object)
		{
			/* Obtain an existing viewController */
			if (!viewController)
			{
				/* No existing controller, obtain a new one */
				viewController = [object viewController];
				if (viewController)
				{ [viewControllerDictionary setObject:viewController forKey:[object description]]; }
			}
			
			/* Set height */
			if ([viewController view])
			{ height = [[viewController view] frame].size.height; }
			else
			{ height = 0.0; }

			
			/* Cache the height */
			[rowHeightDict setObject:[NSNumber numberWithFloat:height] forKey:[NSNumber numberWithInt:rowIndex]];
		}
	}

	return height;
}

#pragma mark "Drag Image Handling"

- (NSImage *)dragImageForRowsWithIndexes:(NSIndexSet *)dragRows 
							tableColumns:(NSArray *)tableColumns
								   event:(NSEvent *)dragEvent 
								  offset:(NSPointPointer)dragImageOffset
{
	/* Create image */
	float rowHeight = 23.0;
	int maxRows;
	float width = NSWidth([self bounds]);
	float height;
	if ([dragRows count] > 5)
	{ 
		height = (5 * rowHeight) + rowHeight; 
		maxRows = 5;
	}
	else
	{ 
		height = [dragRows count] * rowHeight; 
		maxRows = [dragRows count];
	}
	NSImage *dragImage = [[NSImage alloc] initWithSize:NSMakeSize(width,height)];
	[dragImage lockFocus];
	NSBezierPath *path = [LCRoundedBezierPath pathInRect:NSMakeRect(0,0,[dragImage size].width,[dragImage size].height)];
	[[NSColor colorWithCalibratedWhite:0.2 alpha:0.75] setFill];
	[path fill];
	[dragImage unlockFocus];
	
	/* Iterate through rows */
	int i, row;
	for (i=0; i < maxRows; i++)
	{
		/* Get row */
		if (i==0) row = [dragRows firstIndex];
		else row = [dragRows indexGreaterThanIndex:row];
		
		/* Create generic view */
		LCEntity *entity = [[tableContentController arrangedObjects] objectAtIndex:row];
		LCEntityViewController *viewController = [[LCEntityViewController alloc] initGenericForEntity:entity];
		NSImage *viewImage = [[[NSImage alloc] initWithData:[[viewController view] dataWithPDFInsideRect:[[viewController view] bounds]]] autorelease];
		[dragImage lockFocus];
		[viewImage drawInRect:NSMakeRect(0, height - ((i+1) * rowHeight), [viewImage size].width, [viewImage size].height) 
					 fromRect:NSMakeRect(0, 0, [viewImage size].width, [viewImage size].height)
					operation:NSCompositeSourceOver
					 fraction:1.0];
		[dragImage unlockFocus];
		[viewImage autorelease];
	}
	
	/* Check for final row */
	if ([dragRows count] > 5)
	{
		NSRect textRect = NSMakeRect(16, 4, width - 32, rowHeight - 4);
		NSMutableString *str = [NSMutableString stringWithFormat:@"... and %i more... (", [dragRows count] - maxRows];
		for (i=0; i < [dragRows count]; i++)
		{
			if (i==0) row = [dragRows firstIndex];
			else row = [dragRows indexGreaterThanIndex:row];
			LCEntity *entity = [[tableContentController arrangedObjects] objectAtIndex:row];
			
			if (i >= maxRows)
			{
				if (i == maxRows) [str appendFormat:@"%@", [entity desc]]; 
				else if (i == ([dragRows count] - 1)) [str appendFormat:@", %@)", [entity desc]]; 
				else [str appendFormat:@", %@", [entity desc]]; 
			}
		}
		NSShadow *textShadow = [[NSShadow alloc] init]; 
		[textShadow setShadowOffset:NSMakeSize(1.0, -1.0)]; 
		[textShadow setShadowBlurRadius:1];
		[textShadow setShadowColor:[[NSColor blackColor] colorWithAlphaComponent:0.3]];
		NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0], NSForegroundColorAttributeName,
			[NSFont fontWithName:@"Bank Gothic Medium BT" size:11.0], NSFontAttributeName,
			textShadow, NSShadowAttributeName,
			nil];
		[dragImage lockFocus];
		[str drawInRect:textRect withAttributes:attr];
		[dragImage unlockFocus];
		[textShadow release];
	}		

	[dragImage autorelease];
	return dragImage;

//	/* Get object */
//	id object = [[tableContentController arrangedObjects] objectAtIndex:[dragRows firstIndex]];
//	
//	/* Create image */
//	LCEntityViewController *viewController = [object viewController];
//	NSRect r = [[viewController view] bounds];
//	NSImage *dragImage = [[NSImage alloc] initWithSize:NSMakeSize(r.size.width, r.size.height)];
//	[dragImage lockFocus];
//	NSImage *viewImage = [[NSImage alloc] initWithData:[[viewController view] dataWithPDFInsideRect:r]];
//	NSBezierPath *path = [LCRoundedBezierPath pathInRect:NSMakeRect(0,0,[dragImage size].width,[dragImage size].height)];
//	[[NSColor colorWithCalibratedWhite:0.2 alpha:0.6] setFill];
//	[path fill];
//	[viewImage drawInRect:NSMakeRect(0,0,[dragImage size].width, [dragImage size].height) 
//				 fromRect:NSMakeRect(0,0,[viewImage size].width, [viewImage size].height) 
//				operation:NSCompositeSourceOver
//				 fraction:0.9];
//	[dragImage unlockFocus];
//	
//	return dragImage;
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
				if (viewController)
				{
					[viewController removeViewAndContent];
					[rowHeightDict removeObjectForKey:[NSNumber numberWithInt:rowIndex]];
				}
					
				/* No existing (or generic is no longer necessary), Obtain new controller */
				viewController = [object viewController];
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

- (void) setFrameSize:(NSSize)size
{
	[super setFrameSize:size];
}

#pragma mark Menu Items
- (NSMenu *) menuForEvent:(NSEvent *)event
{
	NSArray *selectedObjects = [tableContentController selectedObjects];
	if ([selectedObjects count] < 1 || ![[[[selectedObjects objectAtIndex:0] container] name] isEqualToString:@"service"])
	{
		[serviceMenuItem setEnabled:NO];
	}
	else
	{
		[serviceMenuItem setEnabled:YES];
	}
	return [super menuForEvent:event];
}


@synthesize viewColumn;
@synthesize viewControllerDictionary;
@synthesize containersArrayController;
@synthesize serviceMenuItem;
@synthesize browserController;
@synthesize rowHeightDict;
@end
