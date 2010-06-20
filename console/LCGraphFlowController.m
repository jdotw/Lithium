//
//  LCGraphFlowController.m
//  Lithium Console
//
//  Created by James Wilson on 4/05/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCGraphFlowController.h"

#import "LCGraphFlowCard.h"
#import "LCGraphFlowLayout.h"

@implementation LCGraphFlowController

- (id) init
{
	self = [super init];
	if (!self) return nil;
	
	cards = [[NSMutableArray array] retain];
	cardDictionary = [[NSMutableDictionary dictionary] retain];
	scrollValue = 0.0;
	
	rootLayer = [[CALayer layer] retain];

	LCGraphFlowLayout *cardLayoutManager = [LCGraphFlowLayout new];
	cardLayoutManager.controller = self;
	rootLayer.layoutManager = cardLayoutManager;
	[cardLayoutManager autorelease];
	
	[rootLayer setNeedsLayout];
	
	return self;	
}

- (id) retain
{
	return [super retain];
}

- (void) dealloc
{
	[scroller removeObserver:self forKeyPath:@"doubleValue"];
	[metrics release];
	[cards release];
	[cardDictionary release];
	[rootLayer release];
	[super dealloc];
}

#pragma mark "Metrics"

@synthesize target;
- (void) setTarget:(LCEntity *)value
{
	NSArray *graphable = [value graphableMetrics];
	if (graphable.count > 100)
	{ 
		target = nil; 
		self.metrics = nil;
	}
	else
	{
		[target release];
		target = [value retain];
		self.metrics = graphable;
	}
}

@synthesize metrics;
- (void) setMetrics:(NSArray *)value
{
	/* Set value */
	[metrics release];
	metrics = [value copy];
	
	/* Create cards and layers */
	[self updateCards];
}

#pragma mark "Scroller"

@synthesize focusMetric;
@synthesize scrollValue;
- (void) setScrollValue:(double)value
{	
	if (isnan(value)) return;
	
	/* Get old focus metric */
	int oldCardIndex = (int) ((cards.count - 1) * scrollValue);
	
	scrollValue = value;

	/* Set focus metric (first visible) */
	int cardIndex = (int) ((cards.count - 1) * scrollValue);
	if ((cardIndex != oldCardIndex || !self.focusMetric) && cardIndex < cards.count)
	{
		self.focusMetric = [cards objectAtIndex:cardIndex];

		/* Update the layout */
		[rootLayer setNeedsLayout];
	}
}
- (void) scrollToMetric:(LCMetric *)metric
{
	/* Scroll to the specified metric */
	int index = [metrics indexOfObject:metric];
	if (index == NSNotFound) return;
	[scroller setDoubleValue:(float) index / (float) (metrics.count - 1)];
}
- (void) scrollToObject:(LCObject *)object
{
	NSArray *objectMetrics = [object graphableMetrics];
	if (objectMetrics.count > 0)
	{
		LCMetric *scrollMetric = [objectMetrics objectAtIndex:0];
		int index = [metrics indexOfObject:scrollMetric];
		if (index == NSNotFound) return;
		[scroller setDoubleValue:(float) index / (float) (metrics.count - 1)];		
	}
}

#pragma mark "KVO Methods"

- (void)observeValueForKeyPath:(NSString *)keyPath
					  ofObject:(id)object 
                        change:(NSDictionary *)change
                       context:(void *)context
{
	if (object == scroller)
	{
		self.scrollValue = scroller.doubleValue;
	}
}

#pragma mark "Cards"

@synthesize cards;
- (void) insertObject:(id)obj inCardsAtIndex:(unsigned int)index
{
	[cards insertObject:obj atIndex:index];
}
- (void) removeObjectFromCardsAtIndex:(unsigned int)index
{
	[cards removeObjectAtIndex:index];
}
@synthesize cardDictionary;

- (void) updateCards
{
	/* 
	 * Fill the card deck based on the metrics 
	 */
	
	/* Clear out the old */
	while ([cards count] > 0)
	{
		LCGraphFlowCard *card = [cards objectAtIndex:0];
		[card.cardLayer removeFromSuperlayer];
		[cards removeObjectAtIndex:0];
		[cardDictionary removeObjectForKey:[card.metric uniqueIdentifier]];
	}
	
	/* Create the new */
	for (LCMetric *metric in metrics)
	{
		/* Create card */
		LCGraphFlowCard *card = [LCGraphFlowCard new];
		card.metric = metric;
		card.flowController = self;
		[self insertObject:card inCardsAtIndex:[cards count]];
		[cardDictionary setObject:card forKey:[metric uniqueIdentifier]];
		[card autorelease];

		/* Add to layer */
		[rootLayer addSublayer:card.cardLayer];			
	}
	
	/* Set focus */
	if (cards.count > 0)
	{ 
		/* Set scroller (this will move the focus metric too) */
		[self.scroller setEnabled:YES];
		[self.scroller setHidden:NO];
		self.focusMetric = [cards objectAtIndex:0]; 
		[self.scroller setDoubleValue:0.0];
	}
	else
	{ 
		self.focusMetric = nil; 
		[self.scroller setEnabled:NO];
		[self.scroller setHidden:YES];
	}
	
	[rootLayer setNeedsLayout];
}

@synthesize rootLayer;

@synthesize scroller;
- (void) setScroller:(LCBrowserHorizontalScroller *)value
{
	[scroller removeObserver:self forKeyPath:@"doubleValue"];
	
	scroller = value;
	
	if (scroller)
	{
		[scroller setHidden:YES];
		[scroller setArrowsPosition:NSScrollerArrowsNone];
		[scroller addObserver:self
				   forKeyPath:@"doubleValue"
					  options:(NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld)
					  context:NULL];	
	}
}

@end
