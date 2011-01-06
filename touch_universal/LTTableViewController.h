//
//  LTTableViewController.h
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTTableViewController : UITableViewController 
{
	BOOL isVisible;
}

- (void) refresh;
@property (nonatomic, readonly) BOOL isVisible;

@end
