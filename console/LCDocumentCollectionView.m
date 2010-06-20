//
//  LCDocumentCollectionView.m
//  Lithium Console
//
//  Created by James Wilson on 19/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCDocumentCollectionView.h"
#import "LCDocumentCollectionViewItem.h"
#import "LCDocumentCollectionSubView.h"

@implementation LCDocumentCollectionView

- (void) awakeFromNib
{
	[self setMaxItemSize:self.defaultItemSize];
	[self setMinItemSize:self.defaultItemSize];
	[self setBackgroundColors:[NSArray arrayWithObject:[NSColor clearColor]]];
}

- (NSSize) defaultItemSize
{ 
	return NSMakeSize(171, 212);
}
 
- (NSCollectionViewItem *)newItemForRepresentedObject:(id)object
{
	LCDocumentCollectionViewItem *item = [LCDocumentCollectionViewItem new];
	[item setRepresentedObject:object];
	return item;
}

@synthesize delegate;

@end
