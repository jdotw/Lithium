//
//  LTActionInfoCell.h
//  Lithium
//
//  Created by James Wilson on 5/03/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTAction.h"

@interface LTActionInfoCell : UITableViewCell 
{
	UIImageView *iconImageView;
	UILabel *descLabel;
	UILabel *scriptLabel;
	UILabel *executionLabel;
	UILabel *runLabel;
	UILabel *runCountLabel;
	
	LTAction *action;
}

@property (retain) LTAction *action;

@end
