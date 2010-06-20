//
//  LCBrowserGroupContentController.m
//  Lithium Console
//
//  Created by James Wilson on 30/07/09.
//  Copyright 2009 LithiumCorp. All rights reserved.
//

#import "LCBrowserGroupContentController.h"
#import "LCGroupTreeOutlineEntityItem.h"
#import "LCObjectTreeMetricItem.h"

@interface LCBrowserGroupContentController (private)

- (void) addEntityItemsFromGroup:(LCGroup *)addGroup;
- (void) recursiveAddEntityItemsFromGroup:(LCGroup *)addGroup;
- (void) removeEntityItemsFromGroup:(LCGroup *)removeGroup;
- (void) recursiveRemoveEntityItemsFromGroup:(LCGroup *)removeGroup;
- (void) resetStadiumTargetArray;

@end

@implementation LCBrowserGroupContentController

#pragma mark "Constructors"

- (id) initWithGroup:(LCGroup *)initGroup inBrowser:(LCBrowser2Controller *)initBrowser
{
	self = [self initWithNibName:@"GroupContent" bundle:nil];
	
	if (self)
	{
		/* Set/Create objects */
		self.browser = initBrowser;
		self.group = initGroup;
		
		/* Load NIB */
		[self loadView];
		[groupOutlineView expandAllItemsUsingPreferences];
		
		/* Create Outline Items */
		outlineItems = [[NSMutableArray array] retain];
		[self rebuildOutlineItemArray];
		
		/* Update stadium */
		[self resetStadiumTargetArray];
		
		/* Listen for changes in the container */
		[group addObserver:self 
				forKeyPath:@"children"
				   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
				   context:NULL];			
		
		/* Inspector setup */
		[inspectorController bind:@"target" toObject:self withKeyPath:@"inspectorTarget" options:nil];
		
		/* Observe Device Tree Selection */
		[groupTreeController addObserver:self 
							  forKeyPath:@"selection" 
								 options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
								 context:nil];		
	}
	
	return self;
}

- (void) removedFromBrowserWindow
{
	/* Shutdown and prepare to be autoreleased */
	[group removeObserver:self forKeyPath:@"children"];
	[inspectorController unbind:@"target"];
	[groupTreeController removeObserver:self forKeyPath:@"selection"];
	[super removedFromBrowserWindow];
}

- (void) dealloc
{
	[group release];
	[selectedEntity release];
	[selectedEntities release];
	[outlineItems release];
	[super dealloc];
}

#pragma mark "Selection (KVO Observable)"

@synthesize selectedEntity;

@synthesize selectedEntities;
- (void) setSelectedEntities:(NSArray *)value
{
	[selectedEntities release];
	selectedEntities = [value copy];
	
	/* Check to see if a selection was specified */
	if ([selectedEntities count] > 0)
	{ 
		/* Selection specified, use it. */
		self.selectedEntity = [selectedEntities objectAtIndex:0];		
		self.inspectorTarget = [selectedEntities objectAtIndex:0];
	}
	else
	{ 
		/* No selecton specified, use site */
		self.selectedEntity = nil;
		self.inspectorTarget = nil;
	}
}

@synthesize inspectorTarget;	

#pragma mark "KVO and Notification Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	if (object == groupTreeController)
	{
		/* Device Tree Selection Changed */
		
		if ([[groupTreeController selectedObjects] count] > 0)
		{
			/* Set selection */
			
			id item = [[groupTreeController selectedObjects] objectAtIndex:0];
			if ([item class] == [LCGroupTreeOutlineEntityItem class])
			{
				LCGroupTreeOutlineEntityItem *entityItem = item;
				if (entityItem.entity) self.selectedEntities = [NSArray arrayWithObject:entityItem.entity];
				else self.selectedEntities = nil;
			}
			else if ([item class] == [LCObjectTreeMetricItem class])
			{
				LCObjectTreeMetricItem *metricItem = item;
				if (metricItem.entity) self.selectedEntities = [NSArray arrayWithObject:metricItem.entity];
				else self.selectedEntities = nil;
			}
		}	
		else
		{
			/* No selection */
			self.selectedEntities = nil;
		}	
	}
	else if (object == group)
	{
		/* Change in the container list */
		int changeType = [[change objectForKey:NSKeyValueChangeKindKey] intValue];
		if (changeType == NSKeyValueChangeInsertion)
		{
			for (id item in [change objectForKey:NSKeyValueChangeNewKey])
			{
				if ([[item class] isSubclassOfClass:[LCEntity class]])
				{
					LCGroupTreeOutlineEntityItem *entityItem = [LCGroupTreeOutlineEntityItem new];
					[self insertObject:entityItem inOutlineItemsAtIndex:outlineItems.count];
					[entityItem autorelease];
				}
				else if ([[item class] isSubclassOfClass:[LCGroup class]])
				{
					if ([[NSUserDefaults standardUserDefaults] boolForKey:[NSString stringWithFormat:@"LCGroupTreeOutline-Recursive-%i", group.groupID]])
					{
						[self recursiveAddEntityItemsFromGroup:group];
						/* FIX This doesn't recursively observe */
					}
				}
			}
		}
		else if (changeType == NSKeyValueChangeRemoval)
		{
			for (id item in [change objectForKey:NSKeyValueChangeOldKey])
			{ 
				if ([[item class] isSubclassOfClass:[LCEntity class]])
				{
					NSMutableArray *removeArray = [NSMutableArray array];
					for (LCGroupTreeOutlineEntityItem *entityItem in outlineItems)
					{
						if (entityItem.entity == item) [removeArray addObject:entityItem];
					}
					for (LCGroupTreeOutlineEntityItem *entityItem in removeArray)
					{ [self removeObjectFromOutlineItemsAtIndex:[outlineItems indexOfObject:entityItem]]; }
				}
				else if ([[item class] isSubclassOfClass:[LCGroup class]])
				{
					if ([[NSUserDefaults standardUserDefaults] boolForKey:[NSString stringWithFormat:@"LCGroupTreeOutline-Recursive-%i", group.groupID]])
					{
						[self recursiveRemoveEntityItemsFromGroup:group];
						/* FIX This doesn't recursively unobserve */
					}
				}			
			}
		}		
	}
}

#pragma mark "Outlien Item Methods"

- (void) addEntityItemsFromGroup:(LCGroup *)addGroup
{
	for (id item in addGroup.children)
	{
		if ([[item class] isSubclassOfClass:[LCEntity class]])
		{ 
			LCGroupTreeOutlineEntityItem *entityItem = [[LCGroupTreeOutlineEntityItem alloc] initWithEntity:item];
			[self insertObject:entityItem inOutlineItemsAtIndex:outlineItems.count];
			[entityItem autorelease];
		}
	}
}

- (void) recursiveAddEntityItemsFromGroup:(LCGroup *)addGroup
{
	[self addEntityItemsFromGroup:addGroup];
	for (id item in addGroup.children)
	{
		if ([[item class] isSubclassOfClass:[LCGroup class]])
		{ 
			[self recursiveAddEntityItemsFromGroup:item];
		}
	}	
}

- (void) removeEntityItemsFromGroup:(LCGroup *)removeGroup
{
	for (id item in removeGroup.children)
	{
		if ([[item class] isSubclassOfClass:[LCEntity class]])
		{ 
			for (LCGroupTreeOutlineEntityItem *entityItem in outlineItems)
			{
				if (entityItem.entity == item)
				{ [self removeObjectFromOutlineItemsAtIndex:[outlineItems indexOfObject:entityItem]]; }
			}
		}
	}
}

- (void) recursiveRemoveEntityItemsFromGroup:(LCGroup *)removeGroup
{
	[self removeEntityItemsFromGroup:removeGroup];
	for (id item in removeGroup.children)
	{
		if ([[item class] isSubclassOfClass:[LCGroup class]])
		{ 
			[self recursiveRemoveEntityItemsFromGroup:item];
		}
	}	
}

- (void) rebuildOutlineItemArray
{
	/* Will check user preference to see if 
	 * this should be built recursively or not 
	 */
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:[NSString stringWithFormat:@"LCGroupTreeOutline-Recursive-%i", group.groupID]])
	{
		/* Recursive */
		[self recursiveAddEntityItemsFromGroup:self.group];
	}
	else 
	{
		/* Single-level */
		[self addEntityItemsFromGroup:self.group];
	}
}

- (void) resetStadiumTargetArray
{
	NSMutableArray *targetObjects = [NSMutableArray array];
	for (id item in group.children)
	{
		if ([[item class] isSubclassOfClass:[LCEntity class]])
		{
			LCEntity *entity = (LCEntity *) item;
			if ([entity object] && ![targetObjects containsObject:[entity object]])
			{ [targetObjects addObject:[entity object]]; }
		}
	}
	stadiumController.targetArray = targetObjects;
}

#pragma mark "Items Accessor"

@synthesize outlineItems;

- (void) insertObject:(id)item inOutlineItemsAtIndex:(unsigned int)index
{
	[outlineItems insertObject:item atIndex:index];
	[self resetStadiumTargetArray];
	[groupOutlineView expandAllItemsUsingPreferences];
}
- (void) removeObjectFromOutlineItemsAtIndex:(unsigned int)index
{
	[outlineItems removeObjectAtIndex:index];
	[self resetStadiumTargetArray];
}

#pragma mark "Properties"

@synthesize browser;
@synthesize group;
- (NSWindow *) window
{ 
	NSResponder *nr = self;
	while (nr = [nr nextResponder])
	{ if ([[nr class] isSubclassOfClass:[NSWindow class]]) return (NSWindow *) nr; }
	return nil;
}
- (CGFloat) preferredFixedComponentHeight
{ return 320.0; }

@end
