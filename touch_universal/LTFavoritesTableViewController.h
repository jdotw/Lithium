//
//  LTFavoritesTableViewController.h
//  Lithium
//
//  Created by James Wilson on 16/01/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewController.h"

@class LTEntity;

@interface LTFavoritesTableViewController : LTTableViewController 
{
	UISegmentedControl *displaySegment;
	NSMutableArray *favorites;
	
	NSTimer *refreshTimer;
}

- (void) addToFavorites:(LTEntity *)entity;
- (void) removeFromFavorites:(LTEntity *)entity;

@end
