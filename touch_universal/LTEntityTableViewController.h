//
//  LTEntityTableViewController.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTTableViewController.h"

@interface LTEntityTableViewController : LTTableViewController <UIActionSheetDelegate, UISearchDisplayDelegate, UISearchBarDelegate>
{
	LTEntity *entity;
	NSMutableArray *children;
	
	NSTimer *refreshTimer;
	
	BOOL hasAppeared;
	
	UINavigationController *externalNavigationController;
	
	IBOutlet UISearchBar *searchBar;
	IBOutlet UISearchDisplayController *searchDisplayController;
	NSMutableArray *searchFilteredItems;
	
}

- (id)initWithEntity:(LTEntity *)initEntity;

@property (retain) LTEntity *entity;
@property (nonatomic,assign) UINavigationController *externalNavigationController;

@end
