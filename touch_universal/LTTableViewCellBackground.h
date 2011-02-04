//
//  LTTableViewCellBackground.h
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTTableViewCellBackground : UIView 
{
	UITableViewCell *cell;
	
	int entityState;
}

@property (assign, nonatomic) UITableViewCell *cell;
@property (assign, nonatomic) int entityState;
@property (nonatomic, assign) BOOL drawEntityStateBackgroundColor;

@end
