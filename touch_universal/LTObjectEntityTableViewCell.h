//
//  LTObjectEntityTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 15/02/11.
//  Copyright 2011 LithiumCorp. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "LTHardwareEntityTableViewCell.h"

@class LTEntity;

@interface LTObjectEntityTableViewCell : LTHardwareEntityTableViewCell 
{
    LTEntity *_valueEntity;
}

@end
