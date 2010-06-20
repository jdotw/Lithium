//
//  LCBrowserObjectTableView.m
//  Lithium Console
//
//  Created by James Wilson on 13/03/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserObjectTableView.h"

#import "LCEntityViewController.h"

@implementation LCBrowserObjectTableView

#pragma mark "NIB Awake"

- (void) awakeFromNib
{
	/* Add container content observer */
	[containersArrayController addObserver:self
						  forKeyPath:@"selection" 
							 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
							 context:NULL];
	
	/* Create row height dict */
	rowHeightDict = [[NSMutableDictionary dictionary] retain];
	
	[super awakeFromNib];
}

- (void) dealloc
{
	[containersArrayController removeObserver:self forKeyPath:@"selection"];
	[super dealloc];
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
		[super reloadData];
	}
}

#pragma mark "Row Height"

- (float)tableView:(NSTableView *)tableView heightOfRow:(int)rowIndex
{
	float height = 13.0;
	
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
		id object = [[tableContentController arrangedObjects] objectAtIndex:rowIndex];
		if (object)
		{
			/* Obtain an existing viewController */
			id viewController = [viewControllerDictionary objectForKey:[object description]];
			if (!viewController)
			{
				/* No existing controller, obtain a new one */
				viewController = [object viewController];
				if (viewController)
				{ [viewControllerDictionary setObject:viewController forKey:[object description]]; }
			}
			
			/* Set height */
			if ([viewController view]) {
				height = [[viewController view] frame].size.height;
			}
			else {
				height = 0;
			}

			
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
	/* Get object */
	id object = [[tableContentController arrangedObjects] objectAtIndex:[dragRows firstIndex]];
	
	/* Create image */
	LCEntityViewController *viewController = [object viewController];
	NSRect r = [[viewController view] bounds];
	NSImage *dragImage = [[NSImage alloc] initWithData:[[viewController view] dataWithPDFInsideRect:r]];
	
	return [dragImage autorelease];
}

@synthesize containersArrayController;
@synthesize rowHeightDict;
@end
