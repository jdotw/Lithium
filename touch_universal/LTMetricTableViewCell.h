//
//  LTMetricTableViewCell.h
//  Lithium
//
//  Created by James Wilson on 27/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "LTEntityTableViewCell.h"

@interface LTMetricTableViewCell : LTEntityTableViewCell 
{
	UILabel *valueLabel;
	
	BOOL showFullLocation;
	BOOL showCurrentValue;
}

- (id)initWithReuseIdentifier:(NSString *)reuseIdentifier;

@property (assign) BOOL showFullLocation;
@property (assign) BOOL showCurrentValue;

@end
