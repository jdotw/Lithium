//
//  LCGroup.m
//  Lithium Console
//
//  Created by James Wilson on 9/07/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGroup.h"

@interface LCGroup (private)
- (void) updateOpstate;
@end

@implementation LCGroup

- (id) init
{
	self = [super init];
	
	/* Create ivars */
	self.opState = -1;
	children = [[NSMutableArray array] retain];
	childrenDictionary = [[NSMutableDictionary dictionary] retain];
	recentChildren = [[NSMutableArray array] retain];
	
	/* Setup XML Translation */
	if (!self.xmlTranslation) self.xmlTranslation = [NSMutableDictionary dictionary];
	[self.xmlTranslation setObject:@"groupID" forKey:@"id"];
	[self.xmlTranslation setObject:@"parentID" forKey:@"parent"];
	[self.xmlTranslation setObject:@"opState" forKey:@"opstate"];
	[self.xmlTranslation setObject:@"name" forKey:@"name"];
	[self.xmlTranslation setObject:@"desc" forKey:@"desc"];
	
	return self;
}

- (void) dealloc
{
	for (id item in children)
	{
		[item removeObserver:self forKeyPath:@"opState"];
	}
	[children release];
	[childrenDictionary release];
	[recentChildren release];
	[name release];
	[desc release];
	[customer release];
	[displayString release];
	[super dealloc];
}

#pragma mark "KVO Observing"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if ([keyPath isEqualToString:@"opState"])
	{ [self updateOpstate]; }
}

#pragma mark "Op State"

- (void) updateOpstate
{
	int highestOpstate = -3;
	for (id item in children)
	{
		if ([item respondsToSelector:@selector(opState)])
		{
			if ((int)[item opState] > highestOpstate)
			{ highestOpstate = (int)[item opState]; }
		}
	}
	if (self.opState != highestOpstate) self.opState = highestOpstate;
}

#pragma mark "Properties"

@synthesize groupID;
@synthesize parentID;
@synthesize parent;
@synthesize customer;
@synthesize opState;
@synthesize name;
@synthesize desc;
- (void) setDesc:(NSString *)value
{
	[desc release];
	desc = [value copy];
	
	self.displayString = desc;
	self.sortString = desc;
}
	
@synthesize children;
@synthesize childrenDictionary;
- (void) insertObject:(id)child inChildrenAtIndex:(unsigned int)index
{ 
	[children insertObject:child atIndex:index]; 
	[self updateOpstate];
	[child addObserver:self 
			forKeyPath:@"opState"
			   options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld) 
			   context:NULL];	
}
- (void) removeObjectFromChildrenAtIndex:(unsigned int)index
{ 
	[[children objectAtIndex:index] removeObserver:self forKeyPath:@"opState"];
	[children removeObjectAtIndex:index];
	[self updateOpstate];
}
@synthesize recentChildren;
@synthesize refreshVersion;

#pragma mark "Tree Item Properties"

@synthesize displayString;
	
@synthesize rowHeight;
@synthesize isBrowserTreeLeaf;

- (NSImage *) treeIcon
{ return nil; }

@synthesize refreshInProgress;

- (BOOL) selectable
{ return YES; }

- (NSString *) uniqueIdentifier
{ return [NSString stringWithFormat:@"%@-%@-%i", [self className], self.customer.name, self.groupID]; }

- (BOOL) isGroupTreeLeaf
{
	return NO;
}

@end
