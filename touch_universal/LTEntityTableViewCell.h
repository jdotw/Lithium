//
//  LTEntityTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntity.h"
#import "LTTableViewCell.h"

@interface LTEntityTableViewCell : LTTableViewCell 
{
	LTEntity *entity;
}

@property (retain) LTEntity *entity;

@end
