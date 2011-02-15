//
//  LTEntityTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 26/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTTableViewCell.h"

@class LTEntity;

@interface LTEntityTableViewCell : LTTableViewCell 
{
    
    BOOL showLocation;              // Use the location label to show full location
}

- (id) initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic,retain) LTEntity *entity;
@property (nonatomic,assign) BOOL drawAsRack;        // Enables the 'Rack' style drawing
@property (nonatomic,assign) BOOL showLocation;

@end
