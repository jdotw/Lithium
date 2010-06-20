//
//  LCVRackCollectionView.m
//  Lithium Console
//
//  Created by James Wilson on 23/06/09.
//  Copyright 2009 LithiumCorp Pty Ltd. All rights reserved.
//

#import "LCVRackCollectionView.h"

#import "LCDocumentCollectionView.h"
#import "LCDocumentCollectionViewItem.h"
#import "LCDocumentCollectionSubView.h"

@implementation LCVRackCollectionView

- (NSCollectionViewItem *)newItemForRepresentedObject:(id)object
{
	LCDocumentCollectionViewItem *item = [LCDocumentCollectionViewItem new];
	[NSBundle loadNibNamed:@"VRackCollectionItem" owner:item];
	[item setRepresentedObject:object];
	[(LCDocumentCollectionSubView *)[item view] setDelegate:delegate]; 
	return item;
}

- (NSSize) defaultItemSize
{
	return NSMakeSize(220, 380);
}


@end
