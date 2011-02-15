//
//  LTTableViewController.h
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EGORefreshTableHeaderView.h"

@interface LTTableViewController : UITableViewController <EGORefreshTableHeaderDelegate, UITableViewDelegate, UITableViewDataSource>
{
	BOOL isVisible;
    
    NSTimer *sparkleTimer;
}

- (void) refresh;
- (void) forceRefresh;

@property (nonatomic,readonly) BOOL isVisible;
@property (nonatomic,assign) BOOL pullToRefresh;
@property (nonatomic,readonly) BOOL refreshInProgress;
@property (nonatomic,assign) BOOL reloading;
@property (nonatomic,retain) EGORefreshTableHeaderView *refreshHeaderView;

@end
