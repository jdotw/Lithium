//
//  LTGraphLegendTableViewController.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTGraphLegendTableViewController : UITableViewController 
{
	NSArray *entities;		/* The metrics being shown in the graph */
	int entitiesPerRow;		/* Default is 2 */
}

@property (nonatomic, retain) NSArray *entities;
@property (nonatomic, assign) int entitiesPerRow;

@end
