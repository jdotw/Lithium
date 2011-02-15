//
//  LTTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 2/07/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LTTableViewCell : UITableViewCell 
{
    UILabel *locationLabel;         // Subtitle label for use with a Value style Cell	
}

@property (nonatomic, assign) int entityState;
@property (nonatomic, assign) BOOL drawEntityStateBackgroundColor;
@property (nonatomic,readonly) UILabel *locationLabel;

@end
