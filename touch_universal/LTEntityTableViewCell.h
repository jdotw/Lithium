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
	UILabel *valueLabel;
}

- (id) initWithReuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic,retain) LTEntity *entity;
@property (nonatomic,assign) BOOL showFullLocation;
@property (nonatomic,assign) BOOL showCurrentValue;
@property (nonatomic,assign) BOOL drawAsRack;        // Enables the 'Rack' style drawing

@end
