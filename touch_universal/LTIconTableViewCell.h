//
//  LTIconTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 12/10/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTIconTableViewCell : UITableViewCell 
{
}

+ (int) itemsPerRowAtHeight:(CGFloat)height width:(CGFloat)width;
- (void) removeAllSubviews;

@end
