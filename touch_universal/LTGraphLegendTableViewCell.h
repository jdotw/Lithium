//
//  LTGraphLegendTableViewCell.h
//  LithiumTouchUniversal
//
//  Created by James Wilson on 8/06/10.
//  Copyright 2010 LithiumCorp. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface LTGraphLegendTableViewCell : UITableViewCell 
{
	NSArray *entities;
	
	NSMutableArray *entityViews;
}

@property (nonatomic,retain) NSArray *entities;
- (void) setSwatchColor:(UIColor *)color forEntityAtIndex:(unsigned int)index;

@end
