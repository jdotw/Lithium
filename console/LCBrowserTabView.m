//
//  LCBrowserTabView.m
//  Lithium Console
//
//  Created by James Wilson on 20/10/06.
//  Copyright 2006 LithiumCorp Pty Ltd. All rights reserved. -- FIX Not ready for 5.0
//

#import "LCBrowserTabView.h"


@implementation LCBrowserTabView

- (void) awakeFromNib
{
	selectedTabViewItem = [super selectedTabViewItem];
	NSEnumerator *itemEnum = [[self tabViewItems] objectEnumerator];
	NSTabViewItem *item;
	while (item=[itemEnum nextObject])
	{
		if (![[self subviews] containsObject:[item view]])
		{ [self addSubview:[item view]]; }
		if ([self selectedTabViewItem] != item)
		{ [[item view] setHidden:YES]; }
	}
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	NSEnumerator *itemEnum = [[self tabViewItems] objectEnumerator];
	NSTabViewItem *item;
	while (item=[itemEnum nextObject])
	{
		[[item view] setFrameSize:newSize];
	}
}

- (void)selectTabViewItemAtIndex:(NSInteger)index
{
	/* 
	 * Animate the transition 
	 */
	
	NSView *currentView = [[self selectedTabViewItem] view];
	NSView *nextView = [[self tabViewItemAtIndex:index] view];
	
	NSMutableArray *animations = [NSMutableArray array];	
	NSMutableDictionary *animDict;
	
	if (currentView) 
	{
		animDict = [NSMutableDictionary dictionary];
		[animDict setObject:currentView forKey:NSViewAnimationTargetKey];
		[animDict setObject:NSViewAnimationFadeOutEffect forKey:NSViewAnimationEffectKey];
		[animations addObject:animDict];
	}

	if (nextView)
	{
		animDict = [NSMutableDictionary dictionary];
		[animDict setObject:nextView forKey:NSViewAnimationTargetKey];
		[animDict setObject:NSViewAnimationFadeInEffect forKey:NSViewAnimationEffectKey];
		[animations addObject:animDict];
	}
	
	NSViewAnimation *animation = [[NSViewAnimation alloc] initWithViewAnimations:animations];
	[animation setAnimationBlockingMode:NSAnimationBlocking];
	[animation setDuration:0.25];
	[animation startAnimation];
	[animation autorelease];
	
	selectedTabViewItem = [self tabViewItemAtIndex:index];	
}

- (NSTabViewItem *) selectedTabViewItem
{ return selectedTabViewItem; }

@synthesize selectedTabViewItem;
@end
