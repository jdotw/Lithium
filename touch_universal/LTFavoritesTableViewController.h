//
//  LTFavoritesTableViewController.h
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTTableViewController.h"

@interface LTFavoritesTableViewController : LTTableViewController 
{
	UISegmentedControl *displaySegment;
	NSMutableArray *favorites;
	NSMutableDictionary *descriptorDict;
	NSMutableDictionary *orphanDict;
	NSMutableArray *displayFavorites;
	NSMutableArray *displayOrphans;
}

- (void) addToFavorites:(LTEntity *)entity;
- (void) removeFromFavorites:(LTEntity *)entity;
- (void) bindFavoritesFromDevice:(LTEntity *)device;
- (void) rebuildDisplayFavorites;

@property (nonatomic, retain) NSMutableArray *displayFavorites;


@end
